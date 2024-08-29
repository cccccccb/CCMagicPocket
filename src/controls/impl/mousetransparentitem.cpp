#include "mousetransparentitem.h"

MouseTransparentItem::MouseTransparentItem(QQuickItem *parentItem)
    : QQuickItem(parentItem)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    setAcceptTouchEvents(false);
}

MouseTransparentItem::~MouseTransparentItem()
{

}
