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
    Q_PROPERTY(ActivityManager *activityManager READ activityManager WRITE setActivityManager NOTIFY activityManagerChanged FINAL)

    QML_SINGLETON
    QML_NAMED_ELEMENT(MagicPocket)

public:
    explicit MPQmlInstance(QObject *parent = nullptr);
    ~MPQmlInstance();

    ActivityManager *activityManager() const;
    void setActivityManager(ActivityManager *activityManager);

    bool isAndroid() const;

    Q_INVOKABLE QUrl toMPIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);
    Q_INVOKABLE QUrl toMPShadowUrl(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius, qreal shadowRadius, const QColor &shadowColor);

Q_SIGNALS:
    void activityManagerChanged();

private:
    bool m_isAndroid;
    ActivityManager *m_activityManager;
};

#endif // MAGICPOCKETQMLINSTANCE_H
