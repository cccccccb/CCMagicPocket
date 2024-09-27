#include "mp_preload.h"

#include <QIcon>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQmlApplicationEngine>
#include <QDir>
#include <QSurfaceFormat>
#include <QTimer>

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

    qmlRegisterSingletonType<AnimalModel *>("CCTest", 1, 0, "Model", std::bind(&MPPreload::edge_to_edge_singleton_type_provider, this,
                                                                               std::placeholders::_1, std::placeholders::_2));
}

QUrl MPPreload::preloadModulePath() const
{
    return QUrl("qrc:///CCMagicPocket/qml/PreloadWindow.qml");
}

QObject *MPPreload::edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!animalModel) {
        animalModel = new AnimalModel();
        animalModel->addAnimal(Animal("Wolf", "Medium"));
        animalModel->addAnimal(Animal("Polar bear", "Large"));
        animalModel->addAnimal(Animal("Quoll", "Small"));

        QTimer *timer = new QTimer(this);
        timer->callOnTimeout([=]() {
            static int index = 0;
            animalModel->addAnimal(Animal("Quoll" + QString::number(++index), "Small"));
        });
        timer->start(1000);
    }

    return animalModel;
}
