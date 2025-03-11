#include "iconimageprovider.h"
#include "fast_gaussian_blur_template.h"

#include <QDir>
#include <QIcon>
#include <QHash>
#include <QPainter>
#include <QPixmap>
#include <QPixmapCache>
#include <QStandardPaths>
#include <QUrlQuery>
#include <QThreadPool>
#include <QPainterPath>

IconImageProvider::IconImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{ }

QUrl IconImageProvider::toMPIconUrl(const QString &name, const QColor &color, bool hovered, bool pressed)
{
    QUrl url;
    url.setScheme("image");
    url.setHost("mp.icons");

    QUrlQuery args;
    args.addQueryItem("name", name);
    args.addQueryItem("color", color.name(QColor::HexArgb));
    args.addQueryItem("hovered", QString::number(hovered ? 1 : 0));
    args.addQueryItem("pressed", QString::number(pressed ? 1 : 0));

    url.setQuery(args);

    return url;
}

static QString getStringFromQueryItem(const QString &key, const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue(key);
}

static QColor getColorFromQueryItem(const QString &key, const QString &query)
{
    QUrlQuery args(query);

    return QColor(args.queryItemValue(key));
}

static bool getBoolFromQueryItem(const QString &key, const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue(key).toInt();
}

static qreal getQRealFromQueryItem(const QString &key, const QString &query)
{
    QUrlQuery args(query);

    return args.queryItemValue(key).toDouble();
}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QString &iconName = getStringFromQueryItem("name", id);
    QColor iconColor = getColorFromQueryItem("color", id);
    bool colorIsValid = iconColor.isValid() && iconColor.alpha() > 0;

    if (colorIsValid) {
        if (getBoolFromQueryItem("pressed", id)) {
            iconColor = iconColor.darker(120);
        } else if (getBoolFromQueryItem("hovered", id)) {
            iconColor = iconColor.lighter(120);
        }
    }

    QSize iconSize = requestedSize.isValid() ? requestedSize : QSize(32, 32);
    const QString &sizeString = QStringLiteral("%1x%2").arg(iconSize.width()).arg(iconSize.height());
    QString key = QStringLiteral("_tok_icon_cached:%1:%2").arg(iconName, sizeString);
    if (colorIsValid) {
        key += QLatin1String(":") + iconColor.name(QColor::HexArgb);
    }

    QPixmap pixmap;
    if (!QPixmapCache::find(key, &pixmap)) {
        QIcon icon = QIcon::fromTheme(iconName);
        if (icon.isNull()) {
            return pixmap;
        }

        pixmap = icon.pixmap(iconSize);
        if (pixmap.isNull())
            return pixmap;

        if (colorIsValid) {
            QPainter painter(&pixmap);
            painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
            painter.fillRect(pixmap.rect(), iconColor);
        }

        if (pixmap.size() !=  iconSize) {
            pixmap = pixmap.scaled(iconSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }

        QPixmapCache::insert(key, pixmap);
    }

    if (size) {
        *size = pixmap.size();
    }

    return pixmap;
}

ShadowImageProvider::ShadowImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
    , mCacheSavedThreadPool(new QThreadPool(this))
{
    mCacheSavedThreadPool->setMaxThreadCount(2);
}

QUrl ShadowImageProvider::toMPShadowUrl(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius, qreal shadowRadius,
                                        const QColor &shadowColor, bool surround, qreal offsetX, qreal offsetY, bool inner)
{
    QUrl url;
    url.setScheme("image");
    url.setHost("mp.shadow");

    QUrlQuery args;
    args.addQueryItem("shadowSize", QString::number(shadowSize));
    args.addQueryItem("cornerHRadius", QString::number(cornerHRadius));
    args.addQueryItem("cornerVRadius", QString::number(cornerVRadius));
    args.addQueryItem("shadowRadius", QString::number(shadowRadius));
    args.addQueryItem("shadowColor", shadowColor.name(QColor::HexArgb));
    args.addQueryItem("surround", QString::number(shadowRadius ? 1 : 0));
    args.addQueryItem("offsetX", QString::number(offsetX));
    args.addQueryItem("offsetY", QString::number(offsetY));
    args.addQueryItem("inner", QString::number(inner ? 1 : 0));

    url.setQuery(args);

    return url;
}

static QString generateLocalCachedKey(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius,
                                      qreal shadowRadius, bool inner)
{
    return QString::number(shadowSize) + "_" +
           QString::number(cornerHRadius) + "_" +
           QString::number(cornerVRadius) + "_" +
           QString::number(shadowRadius) + "_" + QString::number(inner ? 1 : 0);
}

static QString shadowCachedDirPath;

static bool findShadowFromLocalCached(const QString &fileName, QImage &result)
{
    if (shadowCachedDirPath.isEmpty())
         shadowCachedDirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/shadow";

    QDir shadowCachedDir(shadowCachedDirPath);
    if (!shadowCachedDir.exists()) {
        return false;
    }

    const auto &targetList = shadowCachedDir.entryInfoList({fileName}, QDir::Files);
    if (targetList.isEmpty())
        return false;

    result = QImage(targetList.first().filePath());
    return !result.isNull();
}

static void saveShadowIntoLocalCached(const QString &fileName, QThreadPool *threadPool, const QImage &result)
{
    if (result.isNull())
        return;

    QDir shadowCachedDir(shadowCachedDirPath);
    if (!shadowCachedDir.exists()) {
        shadowCachedDir.mkpath(shadowCachedDirPath);
    }

    threadPool->start([=]() {
        result.save(shadowCachedDirPath + "/" + fileName, "PNG");
    });
}

static QVector<int> boxesForGauss(double sigma, int size)
{
    double wideal = qSqrt((12 * qPow(sigma, 2) / size) + 1);
    double wl = qFloor(wideal);

    if ((int(wl) % 2) == 0)
        wl--;

    double wu = wl + 2;
    double mideal = (12 * qPow(sigma, 2) - size * qPow(wl, 2)
                     - 4 * size * wl - 3 * size) / (-4 * wl - 4);

    double m = qRound(mideal);
    QVector<int> boxes(size);

    for (int i = 0; i < size; ++i)
        boxes[i] = (i < m ? int(wl) : int(wu));

    return boxes;
}

static void boxBlurForH(uint32_t *origin, uint32_t *dest, int width, int height, int radius)
{
    double iarr = 1.0 / (radius + radius + 1);

    for (int i = 0; i < height; ++i) {
        int ti = i * width;
        int li = ti;
        int ri = ti + radius;

        int fv = origin[ti];
        int lv = origin[ti + width - 1];
        int val = (radius + 1) * fv;

        for (int j = 0; j < radius; ++j) {
            val += origin[ti + j];
        }

        for (int j = 0 ; j <= radius; ++j) {
            val += origin[ri++] - fv;
            dest[ti++] = qRound(val * iarr);
        }

        for (int j = radius + 1; j < width - radius; ++j) {
            val += origin[ri++] - origin[li++];
            dest[ti++] = qRound(val * iarr);
        }

        for (int j = width - radius; j < width; ++j) {
            val += lv - origin[li++];
            dest[ti++] = qRound(val * iarr);
        }
    }
}

static void boxBlurForV(uint32_t *origin, uint32_t *dest, int width, int height, int radius)
{
    qreal iarr = 1.0 / (radius + radius + 1);

    for (int i = 0; i < width; ++i) {
        int ti = i;
        int li = ti;
        int ri = ti + radius * width;
        int fv = origin[ti];
        int lv = origin[ti + width * (height - 1)];
        int val = (radius + 1) * fv;

        for (int j = 0; j < radius; j++)
            val += origin[ti+j * width];

        for (int j = 0; j <= radius; j++) {
            val += origin[ri] - fv;
            dest[ti] = qRound(val * iarr);
            ri += width; ti += width;
        }

        for (int j = radius + 1; j < height - radius; ++j) {
            val += origin[ri] - origin[li];
            dest[ti] = qRound(val * iarr);
            li += width;
            ri += width;
            ti += width;
        }

        for (int j = height - radius; j < height; ++j) {
            val += lv - origin[li];
            dest[ti] = qRound(val * iarr);
            li += width;
            ti += width;
        }
    }
}

static void boxBlur(uint32_t *origin, uint32_t *dest, int width, int height, int radius)
{
    memcpy(dest, origin, width * height * sizeof (uint32_t));
    boxBlurForH(origin, dest, width, height, radius);
    boxBlurForV(dest, origin, width, height, radius);
}

static void doBoxBlur(uint32_t *target, int width, int height, int radius)
{
    QVector<int> boxes = boxesForGauss(radius, 3);
    QScopedArrayPointer<uint32_t> tmpShadowBuffer(new uint32_t[width * height]);

    boxBlur(reinterpret_cast<uint32_t *>(target), tmpShadowBuffer.data(), width, height, (boxes[0] - 1) / 2);
    boxBlur(tmpShadowBuffer.data(), reinterpret_cast<uint32_t *>(target), width, height, (boxes[1] - 1) / 2);
    boxBlur(reinterpret_cast<uint32_t *>(target), tmpShadowBuffer.data(), width, height, (boxes[2] - 1) / 2);
}

static QImage createShadowImage(qreal shadowSize, qreal cornerHRadius, qreal cornerVRadius, qreal shadowRadius,
                                qreal offsetX, qreal offsetY, bool inner)
{
    QImage shadowOrigin(inner ? qRound(shadowSize) + 2 * shadowRadius : qRound(shadowSize),
                        inner ? qRound(shadowSize) + 2 * shadowRadius : qRound(shadowSize), QImage::Format_ARGB32);
    shadowOrigin.fill(inner ? Qt::black : Qt::transparent);

    QPainter painter(&shadowOrigin);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);

    if (inner) {
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.translate(-offsetX, -offsetY);
    }

    if (cornerHRadius == 0 && cornerHRadius == 0) {
        painter.drawRect(shadowOrigin.rect().adjusted(shadowRadius, shadowRadius, -shadowRadius, -shadowRadius));
    } else {
        painter.drawRoundedRect(shadowOrigin.rect().adjusted(shadowRadius, shadowRadius, -shadowRadius, -shadowRadius), cornerHRadius, cornerVRadius);
    }
    painter.end();

    auto originData = shadowOrigin.bits();
    QScopedArrayPointer<uint32_t> tmpData(new uint32_t[shadowOrigin.width() * shadowOrigin.height()]);
    uchar *tmpCharData = reinterpret_cast<uchar *>(tmpData.data());

    fast_gaussian_blur(originData, tmpCharData, shadowOrigin.width(), shadowOrigin.height(), 4, shadowRadius / 2, 3, kWrap);
    return shadowOrigin;
}

QImage ShadowImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qreal shadowSize = getQRealFromQueryItem("shadowSize", id);
    qreal cornerHRadius = getQRealFromQueryItem("cornerHRadius", id);
    qreal cornerVRadius = getQRealFromQueryItem("cornerVRadius", id);
    qreal shadowRadius = getQRealFromQueryItem("shadowRadius", id);
    bool surround = getBoolFromQueryItem("surround", id);
    qreal offsetX = getQRealFromQueryItem("offsetX", id);
    qreal offsetY = getQRealFromQueryItem("offsetY", id);
    bool inner = getBoolFromQueryItem("inner", id);

    QImage target;
    const QString &localCachedKey = generateLocalCachedKey(shadowSize, cornerHRadius, cornerVRadius,
                                                           shadowRadius, inner);
    if (!findShadowFromCurrentCached(localCachedKey, target)) {
        if (!findShadowFromLocalCached(localCachedKey, target)) {
            target = createShadowImage(shadowSize, cornerHRadius, cornerVRadius, shadowRadius, offsetX, offsetY, inner);
            saveShadowIntoLocalCached(localCachedKey, mCacheSavedThreadPool, target);
        }

        saveShadowIntoCurrentCached(localCachedKey, target);
    }

    QPainter painter;

    if (surround && !inner) {
        painter.begin(&target);
        painter.translate(-offsetX, -offsetY);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);

        const QRect roundedRect = target.rect().adjusted(shadowRadius, shadowRadius, -shadowRadius, -shadowRadius);
        if (cornerHRadius == 0 && cornerHRadius == 0) {
            painter.drawRect(roundedRect);
        } else {
            painter.drawRoundedRect(roundedRect, cornerHRadius, cornerVRadius);
        }

        painter.end();
    } else if (inner) {
        QImage innerImage(shadowSize, shadowSize, QImage::Format_ARGB32_Premultiplied);
        innerImage.fill(Qt::transparent);

        painter.begin(&innerImage);
        painter.setRenderHint(QPainter::Antialiasing);

        QPainterPath clipPath;
        if (cornerHRadius == 0 && cornerHRadius == 0) {
            clipPath.addRect(target.rect());
        } else {
            clipPath.addRoundedRect(target.rect(), cornerHRadius, cornerVRadius);
        }
        painter.setClipPath(clipPath);

        painter.translate(-offsetX, -offsetY);
        QRect targetRect = target.rect();
        targetRect.moveCenter(innerImage.rect().center());

        painter.drawImage(targetRect, target);

        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);

        QRect clearRect(0, 0, 1, 1);
        clearRect.moveCenter(innerImage.rect().center());
        painter.drawRect(clearRect);

        painter.end();

        target = innerImage;
    }

    QColor shadowColor = getColorFromQueryItem("shadowColor", id);

    painter.begin(&target);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(target.rect(), shadowColor);
    painter.end();

    if (size)
        *size = target.size();

    return target;
}

bool ShadowImageProvider::findShadowFromCurrentCached(const QString &key, QImage &result)
{
    QImage cached = mCachedImages.value(key);

    if (cached.isNull())
        return false;

    result = cached;
    return true;
}

void ShadowImageProvider::saveShadowIntoCurrentCached(const QString &key, const QImage &target)
{
    mCachedImages.insert(key, target);
}
