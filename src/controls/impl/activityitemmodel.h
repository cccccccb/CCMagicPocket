#ifndef ACTIVITYITEMMODEL_H
#define ACTIVITYITEMMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>
#include <QPointer>
#include <QQuickItem>

class AppStartupModuleGroup;
class ActivityItemModelElement;

class ActivityItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActivityItemModel)
    friend class ActivityManager;
    friend class ActivityManagerPrivate;

public:
    explicit ActivityItemModel(QObject *parent = nullptr);
    ~ActivityItemModel();

    enum ActivityStatus {
        NoStatus,
        Installing,
        Installed,
        Starting,
        Started,
        Stopping,
        Stopped,
        Unstalling,
        Error
    };
    Q_ENUM(ActivityStatus)

    enum ActivityRoles {
        ActivityName = Qt::UserRole + 1,
        DisplayName,
        ItemStatus,
        VersionString,
        IconPath,
        IsActivated,
        PreloadInformation,
        EntityInformation,
        TemplateItem
    };

    bool contains(const QString &activityName) const;
    bool empty() const;

    void addItem(const QSharedPointer<ActivityItemModelElement> &element);
    void removeItem(const QString &activityName);

    Q_INVOKABLE int indexOf(const QString &activityName) const;

Q_SIGNALS:
    void countChanged();

protected:
    int rowCount(const QModelIndex &paren = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItemStatus(const QString &activityName, ActivityStatus status);
    void setItemTemplateItem(const QString &activityName, const QPointer<QQuickItem> &templateItem);
    void setLocalIconPath(const QString &activityName, const QUrl &localIconPath);

    void activateItem(const QString &activityName);
    void clearActivate();

    void setModule(const QString &activityName, const QSharedPointer<AppStartupModuleGroup> &module);
    QSharedPointer<AppStartupModuleGroup> module(const QString &activityName) const;
    QSharedPointer<ActivityItemModelElement> element(const QString &activityName) const;
    QSharedPointer<ActivityItemModelElement> element(const QSharedPointer<AppStartupModuleGroup> &module) const;
    void removeItem(const QSharedPointer<ActivityItemModelElement> &element);

private:
    QList<QSharedPointer<ActivityItemModelElement>> _datas;
    QSharedPointer<ActivityItemModelElement> _activatedElement;
};

struct ActivityItemModelElement
{
    QString activityName;
    QString displayName;
    ActivityItemModel::ActivityStatus status;
    QString versionString;
    QUrl iconPath;
    bool isActivated;
    QPointer<QQuickItem> templateItem;

    QSharedPointer<AppStartupModuleGroup> module;
};

#endif // ACTIVITYITEMMODEL_H
