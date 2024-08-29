#include "mpqmlinstance.h"
#include "iconimageprovider.h"

#include <QQuickWindow>

MPQmlInstance::MPQmlInstance(QObject *parent)
    : QObject{parent}
{}

QUrl MPQmlInstance::toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed)
{
    return IconImageProvider::toTokIconUrl(name, color, hovered, pressed);
}

bool MPQmlInstance::isAndroid() const
{
#ifdef Q_OS_ANDROID
    return true;
#endif
    return false;
}
