#include "planegroupmanager.h"
#include "activityplanegroup.h"

class PlaneTransitionManager : public QQuickTransitionManager
{
public:
    PlaneTransitionManager(const std::function<void (ActivityPlaneGroup *)> & onFinished = nullptr)
        : QQuickTransitionManager()
        , _onFinished(onFinished)
    {}

    void setCurrentGroup(ActivityPlaneGroup *group)
    {
        _currentGroup = group;
    }

protected:
    void finished() override
    {
        if (_onFinished)
            _onFinished(_currentGroup);

        QQuickTransitionManager::finished();
    }

private:
    ActivityPlaneGroup *_currentGroup = nullptr;
    std::function<void (ActivityPlaneGroup *)> _onFinished;
};

PlaneGroupManager::PlaneGroupManager(QObject *parent)
    : QObject{parent}
    , _enterTransitionManager(new PlaneTransitionManager(std::bind(&PlaneGroupManager::onItemEntered, this, std::placeholders::_1) ))
    , _leaveTransitionManager(new PlaneTransitionManager(std::bind(&PlaneGroupManager::onItemLeaved, this, std::placeholders::_1)))
{

}

QQmlListProperty<ActivityPlaneGroup> PlaneGroupManager::roots()
{
    return { this, &_roots,
            &PlaneGroupManager::appendRoot,
            &PlaneGroupManager::countRoot,
            &PlaneGroupManager::atRoot,
            &PlaneGroupManager::clearRoots
    };
}

ActivityPlaneGroup *PlaneGroupManager::currentPlaneGroup() const
{
    return _currentPlaneGroup;
}

void PlaneGroupManager::setCurrentPlaneGroup(ActivityPlaneGroup *planeGroup)
{
    if (!_roots.contains(planeGroup))
        return;

    if (_currentPlaneGroup == planeGroup)
        return;

    auto index = _roots.indexOf(planeGroup);
    updateVisibleRoot(index);
    _currentPlaneGroup = planeGroup;
    Q_EMIT currentPlaneGroupChanged();
}

void PlaneGroupManager::updateVisibleRoot(int index)
{
    if (index < 0 || index >= _roots.size())
        return;

    auto newItem = _roots[index];
    ActivityPlaneGroup *oldItem = nullptr;
    if (_currentPlaneGroup != nullptr) {
        oldItem = _currentPlaneGroup;
        if (oldItem->leave()) {
            _leaveTransitionManager->setCurrentGroup(oldItem);
            _leaveTransitionManager->transition({}, oldItem->leave(), oldItem);
        } else {
            onItemLeaved(oldItem);
        }
    }

    onItemEntered(newItem);

    if (newItem && newItem->enter()) {
        _leaveTransitionManager->setCurrentGroup(newItem);
        _enterTransitionManager->transition({}, newItem->enter(), newItem);
    }
}

void PlaneGroupManager::onItemLeaved(ActivityPlaneGroup *group)
{
    if (group)
        group->setVisible(false);
}

void PlaneGroupManager::onItemEntered(ActivityPlaneGroup *group)
{
    if (group)
        group->setVisible(true);
}

void PlaneGroupManager::appendRoot(QQmlListProperty<ActivityPlaneGroup> *list, ActivityPlaneGroup *item)
{
    auto self = static_cast<PlaneGroupManager*>(list->object);
    self->_roots.append(item);
    item->setVisible(false);

    if (self->_currentPlaneGroup == nullptr) {
        self->setCurrentPlaneGroup(item);
    }
}

qsizetype PlaneGroupManager::countRoot(QQmlListProperty<ActivityPlaneGroup> *list)
{
    return static_cast<PlaneGroupManager*>(list->object)->_roots.size();
}

ActivityPlaneGroup *PlaneGroupManager::atRoot(QQmlListProperty<ActivityPlaneGroup> *list, qsizetype index)
{
    return static_cast<PlaneGroupManager*>(list->object)->_roots.at(index);
}

void PlaneGroupManager::clearRoots(QQmlListProperty<ActivityPlaneGroup> *list)
{
    auto self = static_cast<PlaneGroupManager*>(list->object);
    self->_roots.clear();
}
