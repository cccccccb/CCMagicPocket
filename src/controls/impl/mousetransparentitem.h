#ifndef MOUSETRANSPARENTITEM_H
#define MOUSETRANSPARENTITEM_H

#include <QQuickItem>

class MouseTransparentItem : public QQuickItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MouseTransparentItem)

public:
    explicit MouseTransparentItem(QQuickItem *parentItem = nullptr);
    ~MouseTransparentItem();
};

#endif // MOUSETRANSPARENTITEM_H
