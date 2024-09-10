#include "mpqmlinstance.h"
#include "iconimageprovider.h"
#include "activitymanager.h"

#include <QQuickWindow>

MPQmlInstance::MPQmlInstance(QObject *parent)
    : QObject{parent}
    , m_activityManager(nullptr)
{}

MPQmlInstance::~MPQmlInstance()
{

}

ActivityManager *MPQmlInstance::activityManager() const
{
    if (m_activityManager == nullptr) {
        m_activityManager = new ActivityManager(const_cast<MPQmlInstance *>(this));
    }

    return m_activityManager;
}

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
