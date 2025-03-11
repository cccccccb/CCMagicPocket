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

class QThreadPool;

class ShadowImageProvider : public QQuickImageProvider
{
public:
    ShadowImageProvider();

    static QUrl toMPShadowUrl(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius, qreal shadowRadius,
                              const QColor &shadowColor, bool surround, qreal offsetX, qreal offsetY, bool inner);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    bool findShadowFromCurrentCached(const QString &key, QImage &result);
    void saveShadowIntoCurrentCached(const QString &key, const QImage &target);

private:
    QHash<QString, QImage>      mCachedImages;
    QThreadPool *               mCacheSavedThreadPool;
};

#endif // ICONIMAGEPROVIDER_H
