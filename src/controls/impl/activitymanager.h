#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QQmlComponent>
#include <QScopedPointer>
#include <QQmlListProperty>

class QQuickItem;
class QQuickTransition;

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
    Q_PROPERTY(QQuickItem* runningLayoutView READ runningLayoutView  WRITE setRunningLayoutView NOTIFY runningLayoutViewChanged FINAL)
    Q_PROPERTY(QQuickItem* dockerContainer READ dockerContainer  WRITE setDockerContainer NOTIFY dockerContainerChanged FINAL)
    Q_PROPERTY(QQmlListProperty<QObject> runningActivity READ runningActivity NOTIFY runningActivityChanged FINAL)
    Q_PROPERTY(QQuickItem* currentActivity READ currentActivity NOTIFY currentActivityChanged FINAL)
    Q_PROPERTY(QString currentActivityName READ currentActivityName NOTIFY currentActivityNameChanged FINAL)
    Q_PROPERTY(QQuickTransition* hideTransition READ hideTransition WRITE setHideTransition NOTIFY hideTransitionChanged FINAL)
    Q_PROPERTY(QQuickTransition* showTransition READ showTransition WRITE setShowTransition NOTIFY showTransitionChanged FINAL)
    Q_PROPERTY(QQuickTransition* openTransition READ openTransition WRITE setOpenTransition NOTIFY openTransitionChanged FINAL)
    Q_PROPERTY(QQuickTransition* closeTransition READ closeTransition WRITE setCloseTransition NOTIFY closeTransitionChanged FINAL)
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

    QQuickItem *runningLayoutView() const;
    void setRunningLayoutView(QQuickItem *runningLayoutView);

    QQuickItem *dockerContainer() const;
    void setDockerContainer(QQuickItem *item);

    QQmlListProperty<QObject> runningActivity();
    QQuickItem *currentActivity() const;
    QString currentActivityName() const;

    QQuickTransition *hideTransition() const;
    void setHideTransition(QQuickTransition *hideTransition);

    QQuickTransition *showTransition() const;
    void setShowTransition(QQuickTransition *showTransition);

    QQuickTransition *openTransition() const;
    void setOpenTransition(QQuickTransition *openTransition);

    QQuickTransition *closeTransition() const;
    void setCloseTransition(QQuickTransition *closeTransition);

    Q_INVOKABLE bool isInstalled(const QString &activityName);
    Q_INVOKABLE bool isRunning(const QString &activityName);
    Q_INVOKABLE QQuickItem *runningItemAt(const QString &activityName);

    Q_INVOKABLE void resolve(const QUrl &activityPath);
    Q_INVOKABLE void install(const QUrl &activityPath);
    Q_INVOKABLE void uninstall(const QString &activityName);

    Q_INVOKABLE void open(const QString &activityName);
    Q_INVOKABLE void close(const QString &activityName);

    Q_INVOKABLE void hide(const QString &activityName);
    Q_INVOKABLE void show(const QString &activityName);

    Q_INVOKABLE void layout(const QString &activityName);
    Q_INVOKABLE void exitLayout(const QString &activityName);

    Q_INVOKABLE void activate(const QString &activityName);

Q_SIGNALS:
    void installedPathChanged();
    void runningTemplateChanged();
    void runningContainerChanged();
    void runningActivityChanged();
    void runningLayoutViewChanged();
    void currentActivityChanged();
    void currentActivityNameChanged();
    void dockerContainerChanged();
    void hideTransitionChanged();
    void showTransitionChanged();
    void openTransitionChanged();
    void closeTransitionChanged();

    void resolved(const QUrl &activityPath, const ActivityInformation &info);
    void installed(bool success, const QString &activityName);
    void uninstalled(const QString &activityName);

    void opened(const QString &activityName);
    void closed(const QString &activityName);

private:
    friend class ActivityManagerPrivate;
    Q_PRIVATE_SLOT(dd, void _qq_onModelUnloaded(const QSharedPointer<AppStartupModuleGroup> &))
    Q_PRIVATE_SLOT(dd, void _qq_onModelLoaded(const QSharedPointer<AppStartupModuleGroup> &))
    QScopedPointer<ActivityManagerPrivate> dd;
    QQuickItem *m_runningLayoutView = nullptr;
};

#endif // ACTIVITYMANAGER_H
