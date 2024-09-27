#ifndef MPQMLINSTANCE_H
#define MPQMLINSTANCE_H

#include <QObject>
#include <QUrl>
#include <QQmlComponent>
#include <QColor>

class ActivityManager;
class MPQmlInstance : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT FINAL)
    Q_PROPERTY(ActivityManager *activityManager READ activityManager CONSTANT FINAL)

    QML_SINGLETON
    QML_NAMED_ELEMENT(MagicPocket)

public:
    explicit MPQmlInstance(QObject *parent = nullptr);
    ~MPQmlInstance();

    ActivityManager *activityManager() const;
    bool isAndroid() const;

    Q_INVOKABLE QUrl toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);

private:
    bool m_isAndroid;
    ActivityManager *m_activityManager;
};

#endif // MAGICPOCKETQMLINSTANCE_H
