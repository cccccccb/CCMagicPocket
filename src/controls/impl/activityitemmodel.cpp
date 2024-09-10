#include "activityitemmodel.h"
#include "items/appstartupmodulegroup.h"

ActivityItemModel::ActivityItemModel(QObject *parent)
    : QAbstractListModel{parent}
{}

ActivityItemModel::~ActivityItemModel()
{
    _datas.clear();
    _activatedElement.clear();
}

bool ActivityItemModel::contains(const QString &activityName) const
{
    return std::any_of(_datas.begin(), _datas.end(),
                       [activityName](const QSharedPointer<ActivityItemModelElement> &element) {
            return element->activityName == activityName;
    });
}

void ActivityItemModel::addItem(const QSharedPointer<ActivityItemModelElement> &element)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _datas << element;
    endInsertRows();
}

void ActivityItemModel::removeItem(const QString &activityName)
{
    auto it = std::find_if(_datas.begin(), _datas.end(),
                           [activityName](const QSharedPointer<ActivityItemModelElement> &element) {
        return element->activityName == activityName;
    });

    if (it == _datas.end())
        return;

    removeItem(*it);
}

void ActivityItemModel::removeItem(const QSharedPointer<ActivityItemModelElement> &element)
{
    int indexOf = _datas.indexOf(element);
    if (indexOf == -1)
        return;

    beginRemoveRows(QModelIndex(), indexOf, indexOf);
    _datas.remove(indexOf);
    if (_activatedElement == element)
        _activatedElement.reset();
    endInsertRows();
}

int ActivityItemModel::rowCount(const QModelIndex &parent) const
{
    return _datas.size();
}

QVariant ActivityItemModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _datas.count())
        return QVariant();

    const QSharedPointer<ActivityItemModelElement> &element = _datas[index.row()];
    switch (role) {
    case ActivityItemModel::ActivityName: {
        return element->activityName;
    }
    case ActivityItemModel::ItemName: {
        return element->name;
    }
    case ActivityItemModel::ItemStatus: {
        return element->status;
    }
    case ActivityItemModel::VersionString: {
        return element->versionString;
    }
    case ActivityItemModel::IconPath: {
        return element->iconPath;
    }
    case ActivityItemModel::IsActivated: {
        return element->isActivated;
    }
    case ActivityItemModel::PreloadInformation: {
        return QVariant::fromValue(element->module->preload());
    }
    case ActivityItemModel::EntityInformation: {
        return QVariant::fromValue(element->module->entity());
    }
        break;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> ActivityItemModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { ActivityName, "activityName" },
        { ItemName, "name" },
        { ItemStatus, "status" },
        { VersionString, "versionString" },
        { IconPath, "iconSource" },
        { IsActivated, "isActivated" },
        { PreloadInformation, "preload" },
        { EntityInformation, "entity" },
    };

    return roles;
}

void ActivityItemModel::setItemStatus(const QString &activityName, ActivityStatus status)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.begin(), _datas.end(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->status = status;
    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), {ActivityRoles::ItemStatus});
}

void ActivityItemModel::activateItem(const QString &activityName)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.begin(), _datas.end(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->isActivated = true;
    if (_activatedElement) {
        _activatedElement->isActivated = false;
        int activatedIndex = _datas.indexOf(_activatedElement);

        Q_EMIT dataChanged(index(activatedIndex, 0), index(activatedIndex, 0), {ActivityRoles::IsActivated});
    }

    _activatedElement = (*it);

    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), {ActivityRoles::IsActivated});
}

void ActivityItemModel::setLocalIconPath(const QString &activityName, const QUrl &localIconPath)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.begin(), _datas.end(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->iconPath = localIconPath;
    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), { ActivityRoles::IconPath });
}

void ActivityItemModel::setModule(const QString &activityName, const QSharedPointer<AppStartupModuleGroup> &module)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.begin(), _datas.end(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->module = module;
    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), { ActivityRoles::PreloadInformation, ActivityRoles::EntityInformation });
}

QSharedPointer<AppStartupModuleGroup> ActivityItemModel::module(const QString &activityName) const
{
    auto it = std::find_if(_datas.begin(), _datas.end(), [activityName](const QSharedPointer<ActivityItemModelElement> &element) {
        return element->activityName == activityName;
    });

    if (it == _datas.cend())
        return nullptr;

    return (*it)->module;
}


