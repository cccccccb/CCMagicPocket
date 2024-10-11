#include "mpqmlinstance.h"
#include "iconimageprovider.h"
#include "activitymanager.h"

#include <QQuickWindow>
#include <QtQml>

MPQmlInstance::MPQmlInstance(QObject *parent)
    : QObject{parent}
    , m_activityManager(nullptr)
{}

MPQmlInstance::~MPQmlInstance()
{

}

ActivityManager *MPQmlInstance::activityManager() const
{
    return m_activityManager;
}

void MPQmlInstance::setActivityManager(ActivityManager *activityManager)
{
    if (m_activityManager == activityManager)
        return;

    m_activityManager = activityManager;
    Q_EMIT activityManagerChanged();
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

#include "moc_mpqmlinstance.cpp"
