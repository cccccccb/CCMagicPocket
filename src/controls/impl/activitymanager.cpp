#include "activitymanager.h"
#include "activityitemmodel.h"
#include "activityinformation.h"
#include "activitytemplateitem.h"

#include "zip.h"

#include <appstartupinstance.h>
#include <items/appstartupmoduleinformation.h>
#include <items/appstartupmodulegroup.h>

#include <private/qquicktransition_p.h>
#include <private/qquicktransitionmanager_p_p.h>

#include <QThreadPool>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>
#include <QImageReader>
#include <QBuffer>
#include <QSaveFile>
#include <QQmlContext>
#include <QQuickItem>
#include <QtQml>

#define ACTIVITY_INSTALLED_PATH (QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) \
                                + QChar('/') + QLatin1String("activity"))

#define ACTIVITY_TMP_PATH (QStandardPaths::writableLocation(QStandardPaths::TempLocation) \
                                + QChar('/') + QLatin1String("MagicPacket"))

#define ACTIVITY_TMP_ACTIVITY_PATH (ACTIVITY_TMP_PATH + QChar('/') + QLatin1String("activity"))

#define ACTIVITY_TMP_ICON_PATH (ACTIVITY_TMP_PATH + QChar('/') + QLatin1String("icons"))

#define ACTIVITY_FILE_NAME "Activity.json"

#define ACTIVITY_SURFACE_PROP "surface"

struct ExtractAllCounterData {
    QString activityName;
    ssize_t total;
    ssize_t current;
    std::function<void (qint64, qint64)> callback;
};

class ActivityOpearationTransitionManager : public QQuickTransitionManager
{
public:
    ActivityOpearationTransitionManager(const std::function<void (const QString &)> & onFinished = nullptr)
        : QQuickTransitionManager()
        , _onFinished(onFinished)
    {}

    void setOperationActivity(const QString &activityName)
    {
        _activityName = activityName;
    }

    void setFinishedCallback(const std::function<void (const QString &)> & onFinished)
    {
        _onFinished = onFinished;
    }

protected:
    virtual void finished() override
    {
        if (_onFinished)
            _onFinished(_activityName);

        _activityName.clear();

        QQuickTransitionManager::finished();
    }

private:
    QString _activityName;
    std::function<void (const QString &)> _onFinished;
};

class ActivityManagerPrivate
{
public:
    ActivityManagerPrivate(ActivityManager *qq);

    QByteArray extractSingle(const QString &path, const QString &entryPath);
    bool extractAll(const QString &activityName, const QString &path, const QString &targetPath, const std::function<void (qint64, qint64)> &callback = {});

    ActivityTemplateItem *createTemplate(const QSharedPointer<ActivityItemModelElement> &element);
    QQuickItem *findSurfaceItem(ActivityTemplateItem *item);

    void _qq_onModelLoaded(const QSharedPointer<AppStartupModuleGroup> &module);
    void _qq_onModelUnloaded(const QSharedPointer<AppStartupModuleGroup> &module);

    void activateItem(const QString &activityName);
    void clearCurrentActivity();

    static int onExtractAll(const char *filename, void *arg);
    static size_t onExtractSingle(void *arg, unsigned long long offset, const void *data, size_t size);

    static ActivityInformation parseInformationFromData(const QByteArray &data);
    static ActivityInformation resolveImpl(const QPointer<ActivityManager> &activityManager, const QUrl &activityPath);
    static QSharedPointer<ActivityItemModelElement> convertInformationIntoElement(const ActivityInformation &information);

    ActivityManager *qq = nullptr;

    QString _installedPath;
    QString _currentActivityName;
    QString _defaultLayoutActivityName;

    ActivityItemModel *_installedModel = nullptr;
    ActivityItemModel *_runningModel = nullptr;
    ActivityItemModel *_pinnedModel = nullptr;

    QThreadPool *_managerPool = nullptr;

    QList<QObject *> _runningActivityItems;
    QMap<QString, ActivityTemplateItem *> _runningItemMaps;

    QPointer<QQmlComponent> _runningTemplate;
    QPointer<QQuickItem> _runningDockerContainer;

    ActivityOpearationTransitionManager _openTransitionManager;
    ActivityOpearationTransitionManager _closeTransitionManager;
    ActivityOpearationTransitionManager _minimizedTransitionManager;
    ActivityOpearationTransitionManager _showTransitionManager;
    ActivityOpearationTransitionManager _layoutTransitionManager;
    ActivityOpearationTransitionManager _exitLayoutTransitionManager;
};

size_t ActivityManagerPrivate::onExtractSingle(void *arg, unsigned long long offset, const void *data, size_t size)
{
    QByteArray *buf = (QByteArray *)arg;
    *buf = QByteArray((const char *)data, size);
    return size;
}

int ActivityManagerPrivate::onExtractAll(const char *filename, void *arg)
{
    ExtractAllCounterData *counterData =(ExtractAllCounterData *)arg;
    counterData->current++;
    if (counterData->callback)
        counterData->callback(counterData->current, counterData->total);

    qDebug() << "[ActivityManager] extract activity: " << counterData->activityName << "filename: "
             << filename << "progress: " << counterData->current << "/" << counterData->total
             << QThread::currentThread() << qApp->thread();
    return 0;
}

ActivityManagerPrivate::ActivityManagerPrivate(ActivityManager *qq)
    : qq(qq)
    , _installedModel(new ActivityItemModel(qq))
    , _runningModel(new ActivityItemModel(qq))
    , _pinnedModel(new ActivityItemModel(qq))
    , _managerPool(new QThreadPool(qq))
{
    _managerPool->setMaxThreadCount(1);
    _installedPath = ACTIVITY_INSTALLED_PATH;

    _openTransitionManager.setFinishedCallback([this](const QString &activityName) {

    });

    _closeTransitionManager.setFinishedCallback([this](const QString &activityName) {
        auto module = _runningModel->module(activityName);
        if (!module)
            return;

        AppStartupInstance::instance()->unload(module);
    });

    _minimizedTransitionManager.setFinishedCallback([this](const QString &activityName) {
        clearCurrentActivity();
    });

    _showTransitionManager.setFinishedCallback([this](const QString &activityName) {
    });

    _layoutTransitionManager.setFinishedCallback([this](const QString &activityName) {
        clearCurrentActivity();
    });

    _exitLayoutTransitionManager.setFinishedCallback([this](const QString &activityName) {
        activateItem(activityName);
    });

    AppStartupInstance *instance = AppStartupInstance::instance();
    QObject::connect(instance, SIGNAL(loaded(QSharedPointer<AppStartupModuleGroup>)), qq, SLOT(_qq_onModelLoaded(QSharedPointer<AppStartupModuleGroup>)));
    QObject::connect(instance, SIGNAL(unloaded(QSharedPointer<AppStartupModuleGroup>)), qq, SLOT(_qq_onModelUnloaded(QSharedPointer<AppStartupModuleGroup>)));
}

QByteArray ActivityManagerPrivate::extractSingle(const QString &path, const QString &entryPath)
{
    QByteArray data;

    struct zip_t *zip = zip_open(path.toStdString().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if (!zip)
        return data;

    int ret = zip_entry_open(zip, entryPath.toStdString().c_str());
    if (ret < 0) {
        zip_close(zip);
        return data;
    }

    ret = zip_entry_extract(zip, onExtractSingle, &data);
    if (ret < 0) {
        data.clear();
    }

    zip_entry_close(zip);
    zip_close(zip);
    return data;
}

bool ActivityManagerPrivate::extractAll(const QString &activityName, const QString &path, const QString &targetPath, const std::function<void (qint64, qint64)> &callback)
{
    struct zip_t *zip = zip_open(path.toStdString().c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
    if (!zip)
        return false;

    ExtractAllCounterData data( { activityName, zip_entries_total(zip), 0, callback } );
    zip_close(zip);

    return zip_extract(path.toStdString().c_str(), targetPath.toStdString().c_str(), onExtractAll, &data) == 0;
}

ActivityTemplateItem *ActivityManagerPrivate::createTemplate(const QSharedPointer<ActivityItemModelElement> &element)
{
    if (!this->_runningTemplate) {
        qWarning() << "[Activity Manager]: you need create running tempate before start the activity";
        return nullptr;
    }

    QQmlEngine *engine = qmlEngine(qq->parent());
    if (!engine)
        return nullptr;

    QQmlContext *context = new QQmlContext(engine);

    ActivityTemplateItem *templateItem = qobject_cast<ActivityTemplateItem *>(this->_runningTemplate->beginCreate(context));
    if (!templateItem) {
        qWarning() << "[ActivityManager] running template create fail, use the ActivityTemplateItem instance";
        return nullptr;
    }

    templateItem->setActivityName(element->activityName);
    templateItem->setDisplayName(element->displayName);
    templateItem->setIconPath(element->iconPath);

    engine->setObjectOwnership(templateItem, QQmlEngine::CppOwnership);
    this->_runningTemplate->completeCreate();
    return templateItem;
}

QQuickItem *ActivityManagerPrivate::findSurfaceItem(ActivityTemplateItem *item)
{
    if (!item)
        return nullptr;

    QVariant val = item->property(ACTIVITY_SURFACE_PROP);
    if (!val.isValid())
        return item;

    if (!val.canConvert<QQuickItem *>())
        return item;

    return val.value<QQuickItem *>();
}

void ActivityManagerPrivate::_qq_onModelLoaded(const QSharedPointer<AppStartupModuleGroup> &module)
{
    auto element =  _runningModel->element(module);
    if (element.isNull())
        return;

    Q_EMIT qq->opened(element->activityName);
}

void ActivityManagerPrivate::_qq_onModelUnloaded(const QSharedPointer<AppStartupModuleGroup> &module)
{
    auto element =  _runningModel->element(module);
    if (element.isNull())
        return;

    _runningModel->removeItem(element);

    auto clearActivity = [this, element]() {
        if (_runningItemMaps.contains(element->activityName)) {
            QQuickItem *item = _runningItemMaps.take(element->activityName);
            _runningActivityItems.removeOne(item);
            Q_EMIT qq->runningActivityChanged();
            item->deleteLater();
        }
    };

    clearActivity();
    if (_currentActivityName == element->activityName) {
        clearCurrentActivity();
    }

    Q_EMIT qq->closed(element->activityName);
}

void ActivityManagerPrivate::activateItem(const QString &activityName)
{
    this->_runningModel->activateItem(activityName);

    if (this->_currentActivityName != activityName) {
        if (ActivityTemplateItem *templateItem = this->_runningItemMaps.value(this->_currentActivityName)) {
            templateItem->setIsCurrentItem(false);
        }

        this->_currentActivityName = activityName;
        if (ActivityTemplateItem *templateItem = this->_runningItemMaps.value(this->_currentActivityName)) {
            templateItem->setIsCurrentItem(true);
        }

        Q_EMIT qq->currentActivityChanged();
        Q_EMIT qq->currentActivityNameChanged();
    }
}

void ActivityManagerPrivate::clearCurrentActivity()
{
    if (_currentActivityName.isEmpty())
        return;

    this->_runningModel->clearActivate();
    _currentActivityName.clear();
    Q_EMIT qq->currentActivityChanged();
    Q_EMIT qq->currentActivityNameChanged();
}

ActivityInformation ActivityManagerPrivate::parseInformationFromData(const QByteArray &data)
{
    ActivityInformation information;

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "[Activity Manager] information json data parse failed: " << error.errorString()
                   << ", error number: " << error.error << ", error offset: " << error.offset;
        return information;
    }

    const QJsonObject &obj = document.object();
    information.setId(obj["id"].toString());
    information.setName(obj["name"].toString());
    information.setVersion(obj["version"].toString());
    information.setDescription(obj["description"].toString());
    information.setIconPath(obj["iconPath"].toString());
    information.setPreloadPath(obj["preloadPath"].toString());
    information.setEntityPath(obj["entityPath"].toString());

    return information;
}

ActivityInformation ActivityManagerPrivate::resolveImpl(const QPointer<ActivityManager> &activityManager, const QUrl &activityPath)
{
    ActivityInformation information;

    QString pathString = activityPath.toString();
    const QByteArray &data = activityManager->dd->extractSingle(pathString, ACTIVITY_FILE_NAME);
    if (data.isNull()) {
        return information;
    }

    information = activityManager->dd->parseInformationFromData(data);
    if (!information.isValid()) {
        return information;
    }

    const QString &iconPath = information.iconPath();
    if (iconPath.isEmpty()) {
        return information;
    }

    const QByteArray &iconData = activityManager->dd->extractSingle(pathString, iconPath);
    if (iconData.isEmpty()) {
        qWarning() << "[Activity Manager] load the activity: ["
                   << information.id() << "] icon from path: " << iconPath << " failed!";
        return information;
    }

    QDir().mkpath(ACTIVITY_TMP_ICON_PATH);
    const QString &filePath = ACTIVITY_TMP_ICON_PATH + QChar('/') + information.id();
    QSaveFile tmpIconFile(filePath);
    if (!tmpIconFile.open(QIODevice::WriteOnly)) {
        qWarning() << "[Activity Manager] activity: " << information.id() << "save the icon failed!";
        return information;
    }

    tmpIconFile.write(iconData);
    if (tmpIconFile.commit()) {
        information.setLocalIconPath(filePath);
    } else {
        qWarning() << "[Activity Manager] save the image from the path: " << filePath << " failed!";
    }

    return information;
}

QSharedPointer<ActivityItemModelElement> ActivityManagerPrivate::convertInformationIntoElement(const ActivityInformation &information)
{
    QSharedPointer<ActivityItemModelElement> element(new ActivityItemModelElement);
    element->activityName = information.id();
    element->displayName = information.name();
    element->iconPath = QUrl::fromLocalFile(information.localIconPath());
    element->status = ActivityItemModel::NoStatus;

    element->versionString = information.version();
    return element;
}

ActivityManager::ActivityManager(QObject *parent)
    : QObject{parent}
    , dd(new ActivityManagerPrivate(this))
{}

ActivityManager::~ActivityManager()
{
}

QString ActivityManager::installedPath() const
{
    return dd->_installedPath;
}

void ActivityManager::setInstalledPath(const QString &installedPath)
{
    if (dd->_installedPath == installedPath)
        return;
    dd->_installedPath = installedPath;
    Q_EMIT installedPathChanged();
}

ActivityItemModel *ActivityManager::installModel() const
{
    return dd->_installedModel;
}

ActivityItemModel *ActivityManager::runningModel() const
{
    return dd->_runningModel;
}

ActivityItemModel *ActivityManager::pinnedModel() const
{
    return dd->_pinnedModel;
}

QQmlComponent *ActivityManager::runningTemplate() const
{
    return dd->_runningTemplate;
}

void ActivityManager::setRunningTemplate(QQmlComponent *runingTemplate)
{
    if (dd->_runningTemplate == runingTemplate)
        return;

    dd->_runningTemplate = runingTemplate;
    Q_EMIT runningTemplateChanged();
}

QQuickItem *ActivityManager::dockerContainer() const
{
    return dd->_runningDockerContainer;
}

void ActivityManager::setDockerContainer(QQuickItem *item)
{
    if (dd->_runningDockerContainer == item)
        return;

    dd->_runningDockerContainer = item;
    Q_EMIT dockerContainerChanged();
}

QQmlListProperty<QObject> ActivityManager::runningActivity()
{
    return QQmlListProperty<QObject>(this, &dd->_runningActivityItems);
}

QQuickItem *ActivityManager::currentActivity() const
{
    return dd->_runningItemMaps.value(dd->_currentActivityName);
}

QString ActivityManager::currentActivityName() const
{
    return dd->_currentActivityName;
}

bool ActivityManager::isInstalled(const QString &activityName)
{
    return dd->_installedModel->contains(activityName);
}

bool ActivityManager::isRunning(const QString &activityName)
{
    return dd->_runningModel->contains(activityName);
}

QQuickItem *ActivityManager::runningItemAt(const QString &activityName)
{
    return dd->_runningItemMaps.value(activityName, nullptr);
}

void ActivityManager::resolve(const QUrl &activityPath)
{
    dd->_managerPool->start([activityManager = QPointer<ActivityManager>(this), _activityPath = QUrl(activityPath)]() {
        if (activityManager.isNull())
            return;

        const ActivityInformation &information = ActivityManagerPrivate::resolveImpl(activityManager, _activityPath);
        Q_EMIT activityManager->resolved(_activityPath, information);
    });
}

void ActivityManager::install(const QUrl &activityPath)
{
    dd->_managerPool->start([activityManager = QPointer<ActivityManager>(this), _activityPath = QUrl(activityPath)]() {
        if (activityManager.isNull())
            return;

        const ActivityInformation &information = ActivityManagerPrivate::resolveImpl(activityManager, _activityPath);
        if (!information.isValid())
            return;

        QSharedPointer<ActivityItemModelElement> element = ActivityManagerPrivate::convertInformationIntoElement(information);
        QMetaObject::invokeMethod(activityManager, [activityManager, element]() {
            element->status = ActivityItemModel::Installing;
            activityManager->dd->_installedModel->addItem(element);
        }, Qt::QueuedConnection);

        const QString &activityName = information.id();
        const QString &targetPacketPath = ACTIVITY_INSTALLED_PATH + QChar('/') + QString(activityName).replace(QLatin1Char('.'), QChar('/'));
        const QString &targetPath = targetPacketPath + QChar('/') + information.version();
        QDir().mkpath(targetPath);

        bool success = activityManager->dd->extractAll(activityName, _activityPath.toString(), targetPath);
        ActivityItemModel::ActivityStatus status = ActivityItemModel::Installed;
        if (!success)
            status = ActivityItemModel::Error;

        QMetaObject::invokeMethod(activityManager, [success, activityManager, status, activityName,
                                                    preloadPath = (targetPath + QChar('/') + information.preloadPath()),
                                                    entityPath = (targetPath + QChar('/') + information.entityPath()),
                                                    iconPath = (targetPath + QChar('/') + information.iconPath())]() {
            activityManager->dd->_installedModel->setItemStatus(activityName, status);
            QSharedPointer<AppStartupModuleGroup> module;
            if (success) {
                module.reset(new AppStartupModuleGroup(std::make_pair(AppStartupModuleInformation(preloadPath), AppStartupModuleInformation(entityPath))));
                if (!module->isValid())
                    module.reset();

                activityManager->dd->_installedModel->setLocalIconPath(activityName, QUrl::fromLocalFile(iconPath));
                if (module)
                    activityManager->dd->_installedModel->setModule(activityName, module);
            }

            Q_EMIT activityManager->installed(success && !module.isNull(), activityName);
        }, Qt::QueuedConnection);
    });
}

void ActivityManager::uninstall(const QString &activityName)
{
    if (isRunning(activityName)) {
        close(activityName);
    }

    if (!dd->_installedModel->contains(activityName))
        return;

    dd->_managerPool->start([activityManager = QPointer<ActivityManager>(this), _activityName = QString(activityName)]() {
        if (activityManager.isNull())
            return;

        const QString &activityPath = ACTIVITY_INSTALLED_PATH + QChar('/') + QString(_activityName).replace(QLatin1Char('.'), QChar('/'));
        QDir activityDir(activityPath);
        if (activityDir.exists()) {
            activityDir.removeRecursively();
            while (activityDir.absolutePath() != ACTIVITY_INSTALLED_PATH) {
                if (!activityDir.isEmpty())
                    break;

                activityDir.removeRecursively();
                activityDir.cdUp();
            }
        }

        QMetaObject::invokeMethod(activityManager, [activityManager, _activityName]() {
            activityManager->dd->_installedModel->removeItem(_activityName);
            Q_EMIT activityManager->uninstalled(_activityName);
        }, Qt::QueuedConnection);
    });
}

void ActivityManager::open(const QString &activityName)
{
    if (isRunning(activityName)) {
        activate(activityName);
        return;
    }

    auto module = dd->_installedModel->module(activityName);
    if (!module) {
        qWarning() << "[Activity Manager] No activity name: " << activityName << " found in installed model!";
        return;
    }

    auto element = dd->_installedModel->element(activityName);
    if (!element) {
        //! @todo add error
        return;
    }

    ActivityTemplateItem *templateItem = dd->createTemplate(element);
    if (!templateItem) {
        //! @todo add error
        return;
    }

    element->templateItem = templateItem;
    dd->_runningModel->addItem(element);
    dd->_runningItemMaps.insert(activityName, templateItem);
    dd->_runningActivityItems.append(templateItem);
    Q_EMIT runningActivityChanged();

    module->setSurfaceItem(dd->findSurfaceItem(templateItem));
    dd->activateItem(activityName);

    if (QQuickTransition *tans = templateItem->openTrans()) {
        dd->_openTransitionManager.setOperationActivity(activityName);
        dd->_openTransitionManager.transition({}, tans);
    }

    AppStartupInstance::instance()->load(module);
}

void ActivityManager::close(const QString &activityName)
{
    if (!isRunning(activityName)) {
        return;
    }

    auto element = dd->_installedModel->element(activityName);
    if (!element)
        return;

    auto module = dd->_runningModel->module(activityName);
    if (!module)
        return;

    ActivityTemplateItem *templateItem = dd->_runningItemMaps.value(activityName);
    if (!templateItem) {
        qWarning() << "[ActivityManager] cant find the temlate item named: " << activityName;
        return;
    }

    if (QQuickTransition *tans = templateItem->closeTrans()) {
        if (dd->_closeTransitionManager.isRunning())
            return;

        dd->_closeTransitionManager.setOperationActivity(activityName);
        dd->_closeTransitionManager.transition({}, tans);
    } else {
        AppStartupInstance::instance()->unload(module);
    }
}

void ActivityManager::minimize(const QString &activityName)
{
    if (dd->_currentActivityName != activityName)
        return;

    ActivityTemplateItem *templateItem = dd->_runningItemMaps.value(activityName);
    if (!templateItem) {
        qWarning() << "[ActivityManager] cant find the temlate item named: " << activityName;
        return;
    }

    if (QQuickTransition *tans = templateItem->minimizedTrans()) {
        if (dd->_minimizedTransitionManager.isRunning())
            return;

        dd->_minimizedTransitionManager.setOperationActivity(activityName);
        dd->_minimizedTransitionManager.transition({}, tans);
    } else {
        dd->clearCurrentActivity();
    }
}

void ActivityManager::layout(const QString &activityName)
{
    ActivityTemplateItem *templateItem = dd->_runningItemMaps.value(activityName);
    if (!templateItem) {
        qWarning() << "[ActivityManager] cant find the temlate item named: " << activityName;
        return;
    }

    dd->_defaultLayoutActivityName = activityName;

    if (QQuickTransition *tans = templateItem->layoutTrans()) {
        dd->_layoutTransitionManager.setOperationActivity(activityName);
        dd->_layoutTransitionManager.transition({}, tans);
    } else {
        dd->clearCurrentActivity();
    }
}

void ActivityManager::exitLayout(const QString &activityName)
{
    QString targetActivityName = activityName;
    if (targetActivityName.isEmpty()) {
        targetActivityName = dd->_defaultLayoutActivityName;
    }

    if (targetActivityName.isEmpty())
        return;  // This case only update the view.

    ActivityTemplateItem *templateItem = dd->_runningItemMaps.value(targetActivityName);
    if (!templateItem) {
        qWarning() << "[ActivityManager] cant find the temlate item named: " << activityName;
        return;
    }

    if (QQuickTransition *tans = templateItem->exitLayoutTrans()) {
        dd->_exitLayoutTransitionManager.setOperationActivity(targetActivityName);
        dd->_exitLayoutTransitionManager.transition({}, tans);
    } else {
        dd->activateItem(targetActivityName);
    }
}

void ActivityManager::activate(const QString &activityName)
{
    ActivityTemplateItem *templateItem = dd->_runningItemMaps.value(activityName);
    if (!templateItem) {
        qWarning() << "[ActivityManager] cant find the temlate item named: " << activityName;
        return;
    }

    if (QQuickTransition *tans = templateItem->showTrans()) {
        dd->activateItem(activityName);
        dd->_showTransitionManager.setOperationActivity(activityName);
        dd->_showTransitionManager.transition({}, tans);
    } else {
        dd->activateItem(activityName);
    }
}

#include "moc_activitymanager.cpp"
