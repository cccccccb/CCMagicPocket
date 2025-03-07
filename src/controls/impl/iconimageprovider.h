#ifndef ICONIMAGEPROVIDER_H
#define ICONIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QIcon>
#include <QUrl>
#include <QColor>

class IconImageProvider : public QQuickImageProvider
{
public:
    IconImageProvider();

    static QUrl toMPIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed);
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};

class ShadowImageProvider : public QQuickImageProvider
{
public:
    ShadowImageProvider();

    static QUrl toMPShadowUrl(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius, qreal shadowRadius, const QColor &shadowColor);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};

#endif // ICONIMAGEPROVIDER_H
