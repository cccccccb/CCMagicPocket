#ifndef MPQMLINSTANCE_H
#define MPQMLINSTANCE_H

#include <QObject>
#include <QUrl>
#include <QQmlComponent>
#include <QColor>

class MPQmlInstance : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAndroid READ isAndroid CONSTANT FINAL)

    QML_SINGLETON
    QML_NAMED_ELEMENT(MagicPocket)

public:
    explicit MPQmlInstance(QObject *parent = nullptr);

    bool isAndroid() const;

    Q_INVOKABLE QUrl toTokIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);

private:
    bool m_isAndroid;
};

#endif // MAGICPOCKETQMLINSTANCE_H
