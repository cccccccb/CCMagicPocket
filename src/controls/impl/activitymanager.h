#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QQmlComponent>
#include <QScopedPointer>

class ActivityInformation;
class ActivityItemModel;
class ActivityManagerPrivate;

class ActivityManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString installedPath READ installedPath WRITE setInstalledPath NOTIFY installedPathChanged FINAL)
    Q_PROPERTY(ActivityItemModel* installModel READ installModel CONSTANT FINAL)
    Q_PROPERTY(ActivityItemModel* runningModel READ runningModel CONSTANT FINAL)
    QML_NAMED_ELEMENT(ActivityManager)

public:
    explicit ActivityManager(QObject *parent = nullptr);
    ~ActivityManager();

    QString installedPath() const;
    void setInstalledPath(const QString &newWorkPath);

    ActivityItemModel *installModel() const;
    ActivityItemModel *runningModel() const;

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

    void resolved(const QUrl &activityPath, const ActivityInformation &info);
    void installed(bool success, const QString &activityName);
    void uninstalled(const QString &activityName);

    void started(const QString &activityName);
    void stopped(const QString &activityName);

private:
    friend class ActivityManagerPrivate;
    QScopedPointer<ActivityManagerPrivate> dd;
};

#endif // ACTIVITYMANAGER_H
