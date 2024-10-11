#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QQmlComponent>
#include <QScopedPointer>
#include <QQmlListProperty>

class QQuickItem;
class ActivityInformation;
class ActivityItemModel;
class ActivityManagerPrivate;

class ActivityManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString installedPath READ installedPath WRITE setInstalledPath NOTIFY installedPathChanged FINAL)
    Q_PROPERTY(ActivityItemModel* installModel READ installModel CONSTANT FINAL)
    Q_PROPERTY(ActivityItemModel* runningModel READ runningModel CONSTANT FINAL)
    Q_PROPERTY(QQmlComponent* runningTemplate READ runningTemplate  WRITE setRunningTemplate NOTIFY runningTemplateChanged FINAL)
    Q_PROPERTY(QQuickItem* runningContainer READ runningContainer  WRITE setRunningContainer NOTIFY runningContainerChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QObject> runningActivity READ runningActivity NOTIFY runningActivityChanged FINAL)
    Q_PROPERTY(QString currentActivity READ currentActivity NOTIFY currentActivityChanged FINAL)
    QML_NAMED_ELEMENT(ActivityManager)

public:
    explicit ActivityManager(QObject *parent = nullptr);
    ~ActivityManager();

    QString installedPath() const;
    void setInstalledPath(const QString &installedPath);

    ActivityItemModel *installModel() const;
    ActivityItemModel *runningModel() const;
    ActivityItemModel *pinnedModel() const;

    QQmlComponent *runningTemplate() const;
    void setRunningTemplate(QQmlComponent *runningTemplate);

    QQuickItem *runningContainer() const;
    void setRunningContainer(QQuickItem *item);

    QQmlListProperty<QObject> runningActivity();
    QString currentActivity() const;

    Q_INVOKABLE bool isInstalled(const QString &activityName);
    Q_INVOKABLE bool isRunning(const QString &activityName);

    Q_INVOKABLE void resolve(const QUrl &activityPath);
    Q_INVOKABLE void install(const QUrl &activityPath);
    Q_INVOKABLE void uninstall(const QString &activityName);

    Q_INVOKABLE void start(const QString &activityName);
    Q_INVOKABLE void stop(const QString &activityName);

    Q_INVOKABLE void hide(const QString &activityName);
    Q_INVOKABLE void show(const QString &activityName);
    Q_INVOKABLE void activate(const QString &activityName);

Q_SIGNALS:
    void installedPathChanged();
    void runningTemplateChanged();
    void runningContainerChanged();
    void runningActivityChanged();
    void currentActivityChanged();

    void resolved(const QUrl &activityPath, const ActivityInformation &info);
    void installed(bool success, const QString &activityName);
    void uninstalled(const QString &activityName);

    void started(const QString &activityName);
    void stopped(const QString &activityName);

private:
    friend class ActivityManagerPrivate;
    Q_PRIVATE_SLOT(dd, void _qq_onModelUnloaded(const QSharedPointer<AppStartupModuleGroup> &))
    Q_PRIVATE_SLOT(dd, void _qq_onDelayHided())
    QScopedPointer<ActivityManagerPrivate> dd;
};

#endif // ACTIVITYMANAGER_H
