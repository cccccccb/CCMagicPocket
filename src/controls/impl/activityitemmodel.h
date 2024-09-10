#ifndef ACTIVITYITEMMODEL_H
#define ACTIVITYITEMMODEL_H

#include <QAbstractListModel>
#include <QQmlComponent>

class AppStartupModuleGroup;
class ActivityItemModelElement;

class ActivityItemModel : public QAbstractListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ActivityItemModel)
    friend class ActivityManager;

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
        ItemName,
        ItemStatus,
        VersionString,
        IconPath,
        IsActivated,
        PreloadInformation,
        EntityInformation
    };

    bool contains(const QString &activityName) const;

    void addItem(const QSharedPointer<ActivityItemModelElement> &element);
    void removeItem(const QString &activityName);

protected:
    int rowCount(const QModelIndex &paren = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setItemStatus(const QString &activityName, ActivityStatus status);
    void activateItem(const QString &activityName);
    void setLocalIconPath(const QString &activityName, const QUrl &localIconPath);

    void setModule(const QString &activityName, const QSharedPointer<AppStartupModuleGroup> &module);
    QSharedPointer<AppStartupModuleGroup> module(const QString &activityName) const;
    void removeItem(const QSharedPointer<ActivityItemModelElement> &element);

private:
    QList<QSharedPointer<ActivityItemModelElement>> _datas;
    QSharedPointer<ActivityItemModelElement> _activatedElement;
};

struct ActivityItemModelElement
{
    QString activityName;
    QString name;
    ActivityItemModel::ActivityStatus status;
    QString versionString;
    QUrl iconPath;
    bool isActivated;

    QSharedPointer<AppStartupModuleGroup> module;
};

#endif // ACTIVITYITEMMODEL_H
