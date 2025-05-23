#include "activityitemmodel.h"
#include "items/appstartupmodulegroup.h"

ActivityItemModel::ActivityItemModel(QObject *parent)
    : QAbstractListModel{parent}
{
    addItem(QSharedPointer<ActivityItemModelElement>::create(ActivityItemModelElement{"Firefox", "Firefox", ActivityItemModel::Installed, "0.0.1", QUrl::fromLocalFile("D:/station/workspace/project/CCMagicPocket/src/entity/res/svg/firefox.svg"), false}));
    addItem(QSharedPointer<ActivityItemModelElement>::create(ActivityItemModelElement{"Twitter", "Twitter", ActivityItemModel::Installed, "0.0.1", QUrl::fromLocalFile("D:/station/workspace/project/CCMagicPocket/src/entity/res/svg/twitter.svg"), false}));
    addItem(QSharedPointer<ActivityItemModelElement>::create(ActivityItemModelElement{"Google Chrome", "Google Chrome", ActivityItemModel::Installed, "0.0.1", QUrl::fromLocalFile("D:/station/workspace/project/CCMagicPocket/src/entity/res/svg/google-chrome.svg"), false}));
    addItem(QSharedPointer<ActivityItemModelElement>::create(ActivityItemModelElement{"Wechat", "Wechat", ActivityItemModel::Installed, "0.0.1", QUrl::fromLocalFile("D:/station/workspace/project/CCMagicPocket/src/entity/res/svg/wechat.svg"), false}));
}

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

bool ActivityItemModel::empty() const
{
    return _datas.empty();
}

QSharedPointer<ActivityItemModelElement> ActivityItemModel::element(const QString &activityName) const
{
    auto it = std::find_if(_datas.begin(), _datas.end(),
                           [activityName](const QSharedPointer<ActivityItemModelElement> &element) {
                               return element->activityName == activityName;
    });

    if (it == _datas.end())
        return nullptr;

    return *it;
}

QSharedPointer<ActivityItemModelElement> ActivityItemModel::element(const QSharedPointer<AppStartupModuleGroup> &module) const
{
    auto it = std::find_if(_datas.begin(), _datas.end(),
                           [module](const QSharedPointer<ActivityItemModelElement> &element) {
                               return element->module && element->module->preload() == module->preload()
                                        && element->module->entity() == module->entity();
                           });

    if (it == _datas.end())
        return nullptr;

    return *it;
}

void ActivityItemModel::addItem(const QSharedPointer<ActivityItemModelElement> &element)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _datas.prepend(element);
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

int ActivityItemModel::indexOf(const QString &activityName) const
{
    int indexOf = -1;
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    return indexOf;
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
    endRemoveRows();
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
    case ActivityItemModel::DisplayName: {
        return element->displayName;
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
        return QVariant::fromValue(element->module ? element->module->preload() : AppStartupModuleInformation());
    }
    case ActivityItemModel::EntityInformation: {
        return QVariant::fromValue(element->module ? element->module->entity() : AppStartupModuleInformation());
    }
    case ActivityItemModel::TemplateItem: {
        return QVariant::fromValue(element->templateItem.data());
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
        { DisplayName, "displayName" },
        { ItemStatus, "status" },
        { VersionString, "versionString" },
        { IconPath, "iconSource" },
        { IsActivated, "isActivated" },
        { PreloadInformation, "preload" },
        { EntityInformation, "entity" },
        { TemplateItem, "templateItem" },
    };

    return roles;
}

void ActivityItemModel::setItemStatus(const QString &activityName, ActivityStatus status)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->status = status;
    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), {ActivityRoles::ItemStatus});
}

void ActivityItemModel::setItemTemplateItem(const QString &activityName, const QPointer<QQuickItem> &templateItem)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
        indexOf++;
        return element->activityName == activityName;
    });

    if (indexOf == -1 || it == _datas.cend())
        return;

    (*it)->templateItem = templateItem;
    Q_EMIT dataChanged(index(indexOf, 0), index(indexOf, 0), {ActivityRoles::TemplateItem});
}

void ActivityItemModel::activateItem(const QString &activityName)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
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

void ActivityItemModel::clearActivate()
{
    if (_activatedElement == nullptr)
        return;

    _activatedElement->isActivated = false;
    int activatedIndex = _datas.indexOf(_activatedElement);
    Q_EMIT dataChanged(index(activatedIndex, 0), index(activatedIndex, 0), {ActivityRoles::IsActivated});
    _activatedElement.clear();
}

void ActivityItemModel::setLocalIconPath(const QString &activityName, const QUrl &localIconPath)
{
    int indexOf = -1;
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
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
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName, &indexOf](const QSharedPointer<ActivityItemModelElement> &element) {
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
    auto it = std::find_if(_datas.cbegin(), _datas.cend(), [activityName](const QSharedPointer<ActivityItemModelElement> &element) {
        return element->activityName == activityName;
    });

    if (it == _datas.cend())
        return nullptr;

    return (*it)->module;
}
