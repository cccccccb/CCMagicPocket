#include "activitytemplateitem.h"

ActivityTemplateItem::ActivityTemplateItem(QQuickItem *parent)
    : QQuickItem(parent)
{}

bool ActivityTemplateItem::isCurrentItem() const
{
    return _isCurrentItem;
}

void ActivityTemplateItem::setIsCurrentItem(bool current)
{
    if (_isCurrentItem == current)
        return;

    _isCurrentItem = current;
    Q_EMIT isCurrentItemChanged();
}

QQuickItem *ActivityTemplateItem::surface() const
{
    return _surface;
}

void ActivityTemplateItem::setSurface(QQuickItem *surface)
{
    if (_surface == surface)
        return;

    _surface = surface;
    Q_EMIT surfaceChanged();
}

QString ActivityTemplateItem::activityName() const
{
    return _activityName;
}

void ActivityTemplateItem::setActivityName(const QString &activityName)
{
    if (_activityName == activityName)
        return;

    _activityName = activityName;
}

QString ActivityTemplateItem::displayName() const
{
    return _displayName;
}

void ActivityTemplateItem::setDisplayName(const QString &displayName)
{
    if (_displayName == displayName)
        return;

    _displayName = displayName;
}

QUrl ActivityTemplateItem::iconPath() const
{
    return _iconPath;
}

void ActivityTemplateItem::setIconPath(const QUrl &iconPath)
{
    if (_iconPath == iconPath)
        return;

    _iconPath = iconPath;
}

QQuickTransition *ActivityTemplateItem::openTrans() const
{
    return _openTrans;
}

void ActivityTemplateItem::setOpenTrans(QQuickTransition *openTrans)
{
    if (_openTrans == openTrans)
        return;

    _openTrans = openTrans;
    Q_EMIT openTransChanged();
}

QQuickTransition *ActivityTemplateItem::showTrans() const
{
    return _showTrans;
}

void ActivityTemplateItem::setShowTrans(QQuickTransition *showTrans)
{
    if (_showTrans == showTrans)
        return;

    _showTrans = showTrans;
    Q_EMIT showTransChanged();
}

QQuickTransition *ActivityTemplateItem::minimizedTrans() const
{
    return _minimizedTrans;
}

void ActivityTemplateItem::setMinimizedTrans(QQuickTransition *minimizedTrans)
{
    if (_minimizedTrans == minimizedTrans)
        return;

    _minimizedTrans = minimizedTrans;
    Q_EMIT minimizedTransChanged();
}

QQuickTransition *ActivityTemplateItem::layoutTrans() const
{
    return _layoutTrans;
}

void ActivityTemplateItem::setLayoutTrans(QQuickTransition *layoutTrans)
{
    if (_layoutTrans == layoutTrans)
        return;

    _layoutTrans = layoutTrans;
    Q_EMIT layoutTransChanged();
}

QQuickTransition *ActivityTemplateItem::closeTrans() const
{
    return _closeTrans;
}

void ActivityTemplateItem::setCloseTrans(QQuickTransition *closeTrans)
{
    if (_closeTrans == closeTrans)
        return;

    _closeTrans = closeTrans;
    Q_EMIT closeTransChanged();
}

QQuickTransition *ActivityTemplateItem::exitLayoutTrans() const
{
    return _exitLayoutTrans;
}

void ActivityTemplateItem::setExitLayoutTrans(QQuickTransition *exitLayoutTrans)
{
    if (_exitLayoutTrans == exitLayoutTrans)
        return;

    _exitLayoutTrans = exitLayoutTrans;
    Q_EMIT exitLayoutTransChanged();
}
