#include "activitymanager.h"
#include "activityitemmodel.h"
#include "activityinformation.h"

#include "zip.h"

#include <appstartupinstance.h>
#include <items/appstartupmoduleinformation.h>
#include <items/appstartupmodulegroup.h>

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

#define ACTIVITY_CONTAINER_DELAY_HIDE_PROP "delayHide"

#define DD_SLOT_STRING(ARG) #ARG

struct ExtractAllCounterData {
    QString activityName;
    ssize_t total;
    ssize_t current;
    std::function<void (qint64, qint64)> callback;
};

class ActivityManagerPrivate
{
public:
    ActivityManagerPrivate(ActivityManager *qq)
        : qq(qq)
        , _installedModel(new ActivityItemModel(qq))
        , _runningModel(new ActivityItemModel(qq))
        , _pinnedModel(new ActivityItemModel(qq))
        , _managerPool(new QThreadPool(qq))
    {
        _managerPool->setMaxThreadCount(1);
        _installedPath = ACTIVITY_INSTALLED_PATH;

        AppStartupInstance *instance = AppStartupInstance::instance();
        QObject::connect(instance, SIGNAL(unloaded(QSharedPointer<AppStartupModuleGroup>)), qq, SLOT(_qq_onModelUnloaded(QSharedPointer<AppStartupModuleGroup>)));
    }

    QByteArray extractSingle(const QString &path, const QString &entryPath);
    bool extractAll(const QString &activityName, const QString &path, const QString &targetPath, const std::function<void (qint64, qint64)> &callback = {});
    QQuickItem *createTemplate(const QSharedPointer<ActivityItemModelElement> &element);
    QQuickItem *findSurfaceItem(QQuickItem *item);
    void _qq_onModelUnloaded(const QSharedPointer<AppStartupModuleGroup> &module);
    void _qq_onDelayHided();
    void activateItem(const QString &activityName);
    void bindContainerUnVisible();

    static int onExtractAll(const char *filename, void *arg);
    static size_t onExtractSingle(void *arg, unsigned long long offset, const void *data, size_t size);

    static ActivityInformation parseInformationFromData(const QByteArray &data);
    static ActivityInformation resolveImpl(const QPointer<ActivityManager> &activityManager, const QUrl &activityPath);
    static QSharedPointer<ActivityItemModelElement> convertInformationIntoElement(const ActivityInformation &information);

    ActivityManager *qq = nullptr;

    QString _installedPath;
    QString _currentActivity;
    ActivityItemModel *_installedModel = nullptr;
    ActivityItemModel *_runningModel = nullptr;
    ActivityItemModel *_pinnedModel = nullptr;
    QThreadPool *_managerPool = nullptr;
    QList<QObject *> _runningActivityItems;
    QMap<QString, QQuickItem *> _runningItemMaps;
    QQmlComponent *_runningTemplate = nullptr;
    QQuickItem *_runningContainer = nullptr;
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

QQuickItem *ActivityManagerPrivate::createTemplate(const QSharedPointer<ActivityItemModelElement> &element)
{
    if (!this->_runningTemplate) {
        qWarning() << "[Activity Manager]: you need create running tempate before start the activity";
        return nullptr;
    }

    QQmlEngine *engine = qmlEngine(qq->parent());
    if (!engine)
        return nullptr;

    QQmlContext *context = new QQmlContext(engine);
    context->setContextProperty(QLatin1String("displayName"), element->displayName);
    context->setContextProperty(QLatin1String("activityName"), element->activityName);
    QQuickItem *templateItem = qobject_cast<QQuickItem *>(this->_runningTemplate->create(context));
    if (!templateItem)
        return nullptr;

    engine->setObjectOwnership(templateItem, QQmlEngine::CppOwnership);
    return templateItem;
}

QQuickItem *ActivityManagerPrivate::findSurfaceItem(QQuickItem *item)
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

    if (!_runningContainer) {
        clearActivity();
    } else {
        QObject::connect(_runningContainer, &QQuickItem::visibleChanged, qq, clearActivity, Qt::SingleShotConnection);
        bindContainerUnVisible();
    }
}

void ActivityManagerPrivate::_qq_onDelayHided()
{
    if (_runningContainer) {
        QVariant val = _runningContainer->property(ACTIVITY_CONTAINER_DELAY_HIDE_PROP);
        if (!val.toBool()) {
            _runningContainer->setVisible(false);
        }
    } else {
        _runningContainer->setVisible(false);
    }
}

void ActivityManagerPrivate::activateItem(const QString &activityName)
{
   this->_runningModel->activateItem(activityName);
   this->_currentActivity = activityName;
    Q_EMIT qq->currentActivityChanged();
}

void ActivityManagerPrivate::bindContainerUnVisible()
{
    if (!_runningContainer)
        return;

    QVariant val = _runningContainer->property(ACTIVITY_CONTAINER_DELAY_HIDE_PROP);
    if (!val.isValid()) {
        _qq_onDelayHided();
        return;
    }

    const QMetaObject *mb = _runningContainer->metaObject();
    if (!mb) {
        _qq_onDelayHided();
        return;
    }

    const QMetaProperty &mp = mb->property(mb->indexOfProperty(ACTIVITY_CONTAINER_DELAY_HIDE_PROP));
    if (!mp.isValid() ||!mp.hasNotifySignal()) {
        _qq_onDelayHided();
        return;
    }

    mp.write(_runningContainer, true);
    QMetaObject::connect(_runningContainer, mp.notifySignalIndex(), qq, qq->metaObject()->indexOfSlot(DD_SLOT_STRING(_qq_onDelayHided())), Qt::SingleShotConnection);
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

QQuickItem *ActivityManager::runningContainer() const
{
    return dd->_runningContainer;
}

void ActivityManager::setRunningContainer(QQuickItem *item)
{
    if (dd->_runningContainer == item)
        return;

    dd->_runningContainer = item;
    Q_EMIT runningContainerChanged();
}

QQmlListProperty<QObject> ActivityManager::runningActivity()
{
    return QQmlListProperty<QObject>(this, &dd->_runningActivityItems);
}

QString ActivityManager::currentActivity() const
{
    return dd->_currentActivity;
}

bool ActivityManager::isInstalled(const QString &activityName)
{
    return dd->_installedModel->contains(activityName);
}

bool ActivityManager::isRunning(const QString &activityName)
{
    return dd->_runningModel->contains(activityName);
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
        stop(activityName);
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

void ActivityManager::start(const QString &activityName)
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

    QQuickItem *templateItem = dd->createTemplate(element);
    if (!templateItem) {
        //! @todo add error
        return;
    }

    dd->_runningItemMaps.insert(activityName, templateItem);
    dd->_runningActivityItems.append(templateItem);
    Q_EMIT runningActivityChanged();

    module->setSurfaceItem(dd->findSurfaceItem(templateItem));
    dd->_runningModel->addItem(element);
    if (dd->_runningContainer)
        dd->_runningContainer->setVisible(true);

    AppStartupInstance::instance()->load(module);
    dd->activateItem(activityName);
}

void ActivityManager::stop(const QString &activityName)
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

    AppStartupInstance::instance()->unload(module);
}

void ActivityManager::hide(const QString &activityName)
{
    if (dd->_currentActivity != activityName)
        return;

    dd->bindContainerUnVisible();
    dd->_currentActivity.clear();
    Q_EMIT currentActivityChanged();
}

void ActivityManager::show(const QString &activityName)
{
    if (dd->_runningContainer)
        dd->_runningContainer->setVisible(true);

    dd->activateItem(activityName);
}

void ActivityManager::activate(const QString &activityName)
{
    if (dd->_runningContainer && !dd->_runningContainer->isVisible())
        dd->_runningContainer->setVisible(true);

    dd->activateItem(activityName);
}

#include "moc_activitymanager.cpp"

