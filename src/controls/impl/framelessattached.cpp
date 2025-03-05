#include "framelessattached.h"

#ifdef Q_OS_WIN
#include "frameless_win.h"
#else
#include "frameless.h"
#endif

#include <QQuickItem>

FramelessAttached::FramelessAttached(QQuickWindow *parent)
    : QObject{parent}
#ifdef Q_OS_WIN
    , _frameless(new WinFrameless(parent, this))
#else
    , _frameless(new Frameless(parent, this))
#endif
{
}

QQmlListProperty<QQuickItem> FramelessAttached::moveUnder()
{
#ifdef Q_OS_WIN
    return {this, _frameless->moveUnderPointer()};
#else
    return {this, &_moveUnderContainer};
#endif
}

QQmlListProperty<QQuickItem> FramelessAttached::moveExclude()
{
#ifdef Q_OS_WIN
    return {this, _frameless->moveExcludePointer()};
#else
    return {this, &_moveExcludeContainer};
#endif
}

int FramelessAttached::frameBorder() const
{
#ifdef Q_OS_WIN
#else
    return _frameless->framelessBorder();
#endif
    return 0;
}

void FramelessAttached::setFrameBorder(int newFrameBorder)
{
#ifdef Q_OS_WIN
#else
    if (_frameless->framelessBorder() == newFrameBorder)
        return;

    _frameless->setFramelessBorder(newFrameBorder);
    Q_EMIT frameBorderChanged();
#endif
}

bool FramelessAttached::canWindowResize() const
{
    return _frameless->canWindowResize();
}

void FramelessAttached::setCanWindowResize(bool newCanWindowResize)
{
    if (_frameless->canWindowResize() == newCanWindowResize)
        return;

    _frameless->setCanWindowResize(newCanWindowResize);
    Q_EMIT canWindowResizeChanged();
}


bool FramelessAttached::enabled() const
{
    return _frameless->enabled();
}

void FramelessAttached::setEnabled(bool newEnable)
{
    if (_frameless->enabled() == newEnable)
        return;

#ifndef Q_OS_WIN
    if (newEnable) {
        _frameless->window()->installEventFilter(this);
    } else {
        _frameless->window()->removeEventFilter(this);
    }
#endif

    _frameless->setEnabled(newEnable);
    Q_EMIT enabledChanged();
}

qreal FramelessAttached::contentMargins() const
{
#ifdef Q_OS_WIN
    return 0;
#else
    return _frameless->contentMargins();
#endif
}

void FramelessAttached::setContentMargins(qreal newContentMargins)
{
#ifdef Q_OS_WIN
#else
    if (qFuzzyCompare(_frameless->contentMargins(), newContentMargins))
        return;

    _frameless->setContentMargins(newContentMargins);
    Q_EMIT contentMarginsChanged();
#endif
}

void FramelessAttached::activateWindow()
{
    _frameless->window()->setWindowStates(_frameless->window()->windowStates() & ~Qt::WindowMinimized);
    _frameless->window()->raise();
    _frameless->window()->requestActivate();
}

FramelessAttached *FramelessAttached::qmlAttachedProperties(QObject *object)
{
    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(object)) {
        return new FramelessAttached(window);
    }

    return nullptr;
}

#ifndef Q_OS_WIN
bool FramelessAttached::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == _frameless->window()) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto underIncludeContainer = std::find_if(_moveUnderContainer.begin(), _moveUnderContainer.end(),
                    [](QQuickItem *item) {
                return item->isUnderMouse() && item->isEnabled();
            });

            auto underExcludeContainer = std::find_if(_moveExcludeContainer.begin(), _moveExcludeContainer.end(),
                                          [](QQuickItem *item) {
                return item->isUnderMouse() && item->isEnabled();
            });

            if (underIncludeContainer != _moveUnderContainer.end()) {
                _frameless->setCanWindowMove((underExcludeContainer == _moveExcludeContainer.end())
                                              || ((*underIncludeContainer)->isAncestorOf((*underExcludeContainer))
                                                      ? false
                                                      : ((*underIncludeContainer)->z() > (*underExcludeContainer)->z())));
            } else {
                _frameless->setCanWindowMove(false);
            }
        }

        _frameless->targetEvent(event);
    }

    return QObject::eventFilter(watched, event);
}
#endif
