#include "mp_preload.h"

#include <QIcon>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QSurfaceFormat>

static QStringList buildinPluginPaths()
{
    QStringList result = { QLibraryInfo::path(QLibraryInfo::DataPath) + "/modules" };
    const auto ccPluginPath = qgetenv("MP_QML_MODULE_PATH");
    if (!ccPluginPath.isEmpty())
        result.append(ccPluginPath);

#ifdef MP_QML_MODULE_PATH
    result.append(MP_QML_MODULE_PATH);
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
    qGuiApp->setApplicationName("Magic Pocket");
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

QUrl MPPreload::preloadModulePath() const
{
    return QUrl("qrc:///CCMagicPocket/qml/PreloadWindow.qml");
}
