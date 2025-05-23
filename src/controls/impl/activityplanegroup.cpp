#include "activityplanegroup.h"
#include "activityplaneitem.h"

class PlanePushTransitionManager : public QQuickTransitionManager
{
public:
    using QQuickTransitionManager::QQuickTransitionManager;

    void setFinishedCallback(const std::function<void()> &finished) {
        _finished = finished;
    }

protected:
    void finished() override {
        if (_finished) {
            _finished();
        }

        QQuickTransitionManager::finished();
    }

private:
    std::function<void()> _finished;
};

ActivityPlaneGroup::ActivityPlaneGroup(QQuickItem *parent)
    : QQuickItem(parent)
    , _pushTransitionManager(new PlanePushTransitionManager())
    , _popTransitionManager(new PlanePushTransitionManager())
{
}

ActivityPlaneGroup::~ActivityPlaneGroup()
{

}

ActivityPlaneItem *ActivityPlaneGroup::windowPlane() const
{
    return _windowPlane;
}

void ActivityPlaneGroup::setWindowPlane(ActivityPlaneItem *windowPlane)
{
    if (_windowPlane == windowPlane)
        return;

    if (windowPlane->level() != ActivityPlaneItem::UnknowLevel
        && windowPlane->level() != ActivityPlaneItem::WindowLevel) {
        qWarning() << "[ActivityPlaneGroup] An existing plane is set to the window plane, the plane level is "
                   << windowPlane->level();
        return;
    }

    _windowPlane = windowPlane;
    _windowPlane->setLevel(ActivityPlaneItem::WindowLevel);
    _windowPlane->setGroup(this);
    Q_EMIT windowPlaneChanged();
}

ActivityPlaneItem *ActivityPlaneGroup::fullscreenPlane() const
{
    return _fullscreenPlane;
}

void ActivityPlaneGroup::setFullscreenPlane(ActivityPlaneItem *fullscreenPlane)
{
    if (_fullscreenPlane == fullscreenPlane)
        return;

    if (fullscreenPlane->level() != ActivityPlaneItem::UnknowLevel
        && fullscreenPlane->level() != ActivityPlaneItem::FullscreenLevel) {
        qWarning() << "[ActivityPlaneGroup] An existing plane is set to the fullscreen plane, the plane level is "
                   << fullscreenPlane->level();
        return;
    }

    _fullscreenPlane = fullscreenPlane;
    _fullscreenPlane->setLevel(ActivityPlaneItem::FullscreenLevel);
    _fullscreenPlane->setGroup(this);
    Q_EMIT fullscreenPlaneChanged();
}

ActivityPlaneItem *ActivityPlaneGroup::topPlane() const
{
    return _topPlane;
}

void ActivityPlaneGroup::setTopPlane(ActivityPlaneItem *topPlane)
{
    if (_topPlane == topPlane)
        return;

    if (topPlane->level() != ActivityPlaneItem::UnknowLevel
        && topPlane->level() != ActivityPlaneItem::TopLevel) {
        qWarning() << "[ActivityPlaneGroup] An existing plane is set to the top plane, the plane level is "
                   << topPlane->level();
        return;
    }

    _topPlane = topPlane;
    _topPlane->setLevel(ActivityPlaneItem::TopLevel);
    _topPlane->setGroup(this);
    Q_EMIT topPlaneChanged();
}

ActivityPlaneItem *ActivityPlaneGroup::layoutPlane() const
{
    return _layoutPlane;
}

void ActivityPlaneGroup::setLayoutPlane(ActivityPlaneItem *layoutPlane)
{
    if (_layoutPlane == layoutPlane)
        return;

    if (layoutPlane->level() != ActivityPlaneItem::UnknowLevel
        && layoutPlane->level() != ActivityPlaneItem::LayoutLevel) {
        qWarning() << "[ActivityPlaneGroup] An existing plane is set to the layout plane, the plane level is "
                   << layoutPlane->level();
        return;
    }

    _layoutPlane = layoutPlane;
    _layoutPlane->setLevel(ActivityPlaneItem::LayoutLevel);
    _layoutPlane->setGroup(this);
    Q_EMIT layoutPlaneChanged();
}

ActivityPlaneItem *ActivityPlaneGroup::overlayPlane() const
{
    return _overlayPlane;
}

void ActivityPlaneGroup::setOverlayPlane(ActivityPlaneItem *overlayPlane)
{
    if (_overlayPlane == overlayPlane)
        return;

    if (_overlayPlane->level() != ActivityPlaneItem::UnknowLevel
        && _overlayPlane->level() != ActivityPlaneItem::OverlayLevel) {
        qWarning() << "[ActivityPlaneGroup] An existing plane is set to the overlay plane, the plane level is "
                   << _overlayPlane->level();
        return;
    }

    _overlayPlane = overlayPlane;
    _overlayPlane->setLevel(ActivityPlaneItem::OverlayLevel);
    _overlayPlane->setGroup(this);
    Q_EMIT overlayPlaneChanged();
}

QQuickTransition *ActivityPlaneGroup::enter() const
{
    return _enter;
}

void ActivityPlaneGroup::setEnter(QQuickTransition *trans)
{
    if (_enter == trans)
        return;

    _enter = trans;
    Q_EMIT enterChanged();
}

QQuickTransition *ActivityPlaneGroup::leave() const
{
    return _leave;
}

void ActivityPlaneGroup::setLeave(QQuickTransition *trans)
{
    if (_leave == trans)
        return;

    _leave = trans;
    Q_EMIT leaveChanged();
}

ActivityPlaneItem *ActivityPlaneGroup::currentPlane() const
{
    return _currentPlane;
}

void ActivityPlaneGroup::setCurrentPlane(ActivityPlaneItem *currentPlane)
{
    if (_currentPlane == currentPlane)
        return;

    if (_currentPlane) {
        bool up = currentPlane && _currentPlane->level() < currentPlane->level();
        QQuickTransition *popTrans = up ? _currentPlane->popUp() : _currentPlane->popDown();

        if (popTrans) {
            if (_popTransitionManager->isRunning())
                _popTransitionManager->cancel();

            auto oldPlane = _currentPlane;
            _popTransitionManager->setFinishedCallback([oldPlane]() {
                oldPlane->setVisible(false);
            });

            _popTransitionManager->transition({}, popTrans, _currentPlane);
        } else {
            _currentPlane->setVisible(false);
        }
    }

    if (currentPlane) {
        currentPlane->setVisible(true);

        bool up = _currentPlane && _currentPlane->level() > currentPlane->level();
        QQuickTransition *pushTrans = up ? currentPlane->pushUp() : currentPlane->pushDown();

        if (pushTrans) {
            if (_pushTransitionManager->isRunning())
                _pushTransitionManager->cancel();

            // _pushTransitionManager->setFinishedCallback([=]() {
            //     _currentPlane = currentPlane;
            //     Q_EMIT currentPlaneChanged();
            // });
            _pushTransitionManager->transition({}, pushTrans, currentPlane);
            // return;
        }
    }

    _currentPlane = currentPlane;
    Q_EMIT currentPlaneChanged();
}
