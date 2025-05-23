#include "activityplaneitem.h"

ActivityPlaneItem::ActivityPlaneItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setVisible(false);
}

ActivityPlaneItem::~ActivityPlaneItem()
{

}

ActivityPlaneItem::PlaneLevel ActivityPlaneItem::level() const
{
    return _level;
}

QQuickTransition *ActivityPlaneItem::pushUp() const
{
    return _pushUpTrans;
}

void ActivityPlaneItem::setPushUp(QQuickTransition *push)
{
    if (_pushUpTrans == push)
        return;

    _pushUpTrans = push;
    Q_EMIT pushUpChanged();
}

QQuickTransition *ActivityPlaneItem::popUp() const
{
    return _popUpTrans;
}

void ActivityPlaneItem::setPopUp(QQuickTransition *pop)
{
    if (_popUpTrans == pop)
        return;

    _popUpTrans = pop;
    Q_EMIT popUpChanged();
}

QQuickTransition *ActivityPlaneItem::pushDown() const
{
    return _pushDownTrans;
}

void ActivityPlaneItem::setPushDown(QQuickTransition *push)
{
    if (_pushDownTrans == push)
        return;

    _pushDownTrans = push;
    Q_EMIT pushDownChanged();
}

QQuickTransition *ActivityPlaneItem::popDown() const
{
    return _popDownTrans;
}

void ActivityPlaneItem::setPopDown(QQuickTransition *pop)
{
    if (_popDownTrans == pop)
        return;

    _popDownTrans = pop;
    Q_EMIT popDownChanged();
}

void ActivityPlaneItem::setLevel(PlaneLevel level)
{
    if (_level == level)
        return;

    _level = level;
    setZ(level);
    Q_EMIT levelChanged();
}

void ActivityPlaneItem::setGroup(ActivityPlaneGroup *group)
{
    if (_group == group)
        return;

    _group = group;
    Q_EMIT groupChanged();
}

ActivityPlaneGroup *ActivityPlaneItem::group() const
{
    return _group;
}
