#include "mp_preload.h"

#include <QIcon>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QSurfaceFormat>

static QStringList buildinPluginPaths()
{
    QStringList result = { QLibraryInfo::path(QLibraryInfo::DataPath) + "/plugins" };
    const auto ccPluginPath = qgetenv("MP_QML_PLUGIN_PATH");
    if (!ccPluginPath.isEmpty())
        result.append(ccPluginPath);

#ifdef MP_QML_PLUGIN_PATH
    result.append(MP_QML_PLUGIN_PATH);
#endif

    return result;
}

MPPreload::MPPreload(QObject *parent)
    : QObject{parent}
{}

MPPreload::~MPPreload()
{

}

void MPPreload::aboutToPreload(QQmlApplicationEngine *engine)
{
    qGuiApp->setApplicationName("MagicPocket");
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    if (!format.hasAlpha() || format.alphaBufferSize() != 8) {
        format.setAlphaBufferSize(8);
    }

    QSurfaceFormat::setDefaultFormat(format);

    const auto &pluginList = buildinPluginPaths();
    for (const auto &path : pluginList) {
        engine->addPluginPath(path);
    }
}

QUrl MPPreload::preloadComponentPath() const
{
    return QUrl("qrc:///qml/PreloadWindow.qml");
}
