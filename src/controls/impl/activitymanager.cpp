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

#define ACTIVITY_INSTALLED_PATH (QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) \
                                + QChar('/') + QLatin1String("activity"))

#define ACTIVITY_TMP_PATH (QStandardPaths::writableLocation(QStandardPaths::TempLocation) \
                                + QChar('/') + QLatin1String("MagicPacket"))

#define ACTIVITY_TMP_ACTIVITY_PATH (ACTIVITY_TMP_PATH + QChar('/') + QLatin1String("activity"))

#define ACTIVITY_TMP_ICON_PATH (ACTIVITY_TMP_PATH + QChar('/') + QLatin1String("icons"))

#define ACTIVITY_FILE_NAME "Activity.json"

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
        , _managerPool(new QThreadPool(qq))
    {
        _managerPool->setMaxThreadCount(1);
        _installedPath = ACTIVITY_INSTALLED_PATH;
    }

    static int onExtractAll(const char *filename, void *arg);
    static size_t onExtractSingle(void *arg, unsigned long long offset, const void *data, size_t size);
    QByteArray extractSingle(const QString &path, const QString &entryPath);
    bool extractAll(const QString &activityName, const QString &path, const QString &targetPath, const std::function<void (qint64, qint64)> &callback = {});

    static ActivityInformation parseInformationFromData(const QByteArray &data);
    static ActivityInformation resolveImpl(const QPointer<ActivityManager> &activityManager, const QUrl &activityPath);
    static QSharedPointer<ActivityItemModelElement> convertInformationIntoElement(const ActivityInformation &information);

    ActivityManager *qq = nullptr;

    QString _installedPath;
    ActivityItemModel *_installedModel = nullptr;
    ActivityItemModel *_runningModel = nullptr;
    QThreadPool *_managerPool = nullptr;
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
             << filename << "progress: " << counterData->current << "/" << counterData->total;
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
    element->name = information.name();
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
    emit installedPathChanged();
}

ActivityItemModel *ActivityManager::installModel() const
{
    return dd->_installedModel;
}

ActivityItemModel *ActivityManager::runningModel() const
{
    return dd->_runningModel;
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
        const QString &targetPath = ACTIVITY_INSTALLED_PATH + QChar('/') + QString(activityName).replace(QLatin1Char('.'), QChar('/'));
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

            Q_EMIT activityManager->installed(success && module.isNull(), activityName);
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
        return;
    }

    auto module = dd->_runningModel->module(activityName);
    if (module) {
        activate(activityName);
        return;
    }

    // module->setSurfaceItem();
    AppStartupInstance::instance()->load(module);
}

void ActivityManager::stop(const QString &activityName)
{
    if (!isRunning(activityName)) {
        return;
    }

    auto module = dd->_runningModel->module(activityName);
    if (!module)
        return;

    AppStartupInstance::instance()->unload(module);
}

void ActivityManager::hide(const QString &activityName)
{

}

void ActivityManager::show(const QString &activityName)
{

}

void ActivityManager::activate(const QString &activityName)
{

}
