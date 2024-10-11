#include "mp_plugin.h"
#include "impl/framelessattached.h"
#include "impl/edgetoedgemodehelper.h"
#include "impl/iconimageprovider.h"
#include "impl/mpqmlinstance.h"
#include "impl/mousetransparentitem.h"
#include "impl/activityinformation.h"
#include "impl/activityitemmodel.h"
#include "impl/activitymoduleinformation.h"
#include "impl/activitymanager.h"

#include <qqml.h>
#include <QDebug>

MPControlExtension::MPControlExtension(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

MPControlExtension::~MPControlExtension()
{
    if (edgeToEdgeHelper) {
        delete edgeToEdgeHelper;
        edgeToEdgeHelper = nullptr;
    }

    if (mpInstance) {
        delete mpInstance;
        mpInstance = nullptr;
    }
}

QObject *MPControlExtension::edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!edgeToEdgeHelper)
        edgeToEdgeHelper = new EdgeToEdgeModeHelper;

    return edgeToEdgeHelper;
}

QObject *MPControlExtension::mp_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    if (!mpInstance)
        mpInstance = new MPQmlInstance;

    return mpInstance;
}

void MPControlExtension::registerTypes(const char *uri)
{
    QByteArray implUri = QByteArray(uri).append(".impl");
    qmlRegisterModule(implUri.constData(), 1, 0);

    // @uri CCMagicPocket.impl
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(implUri, 1, 0, "EdgeToEdge", std::bind(&MPControlExtension::edge_to_edge_singleton_type_provider, this,
                                                                                            std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<MPQmlInstance *>(implUri, 1, 0, "MagicPocket", std::bind(&MPControlExtension::mp_singleton_type_provider, this,
                                                                                       std::placeholders::_1, std::placeholders::_2));
    qmlRegisterType<ActivityManager>(implUri, 1, 0, "ActivityManager");
    // @uri CCMagicPocket
    qmlRegisterSingletonType<EdgeToEdgeModeHelper *>(uri, 1, 0, "EdgeToEdge", std::bind(&MPControlExtension::edge_to_edge_singleton_type_provider, this,
                                                                                        std::placeholders::_1, std::placeholders::_2));
    qmlRegisterSingletonType<MPQmlInstance *>(uri, 1, 0, "MagicPocket", std::bind(&MPControlExtension::mp_singleton_type_provider, this,
                                                                                   std::placeholders::_1, std::placeholders::_2));
    qmlRegisterUncreatableType<FramelessAttached>(uri, 1, 0, "Frameless", "Frameless is an abstract type that is only available as an attached property.");
    qmlRegisterType<MouseTransparentItem>(uri, 1, 0, "MouseTransparentItem");
    qmlRegisterType<ActivityItemModel>(uri, 1, 0, "ActivityItemModel");

    QString qmlUriPrefix(QLatin1String("qrc:/") + QString(uri).replace(".", "/") + QLatin1String("/modules"));

    qmlRegisterSingletonType(QUrl(qmlUriPrefix + "/Style/Style.qml"), uri, 1, 0, "Style");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/CheckedAnimateButton.qml"), uri, 1, 0, "CheckedAnimateButton");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/CustomButton.qml"), uri, 1, 0, "CustomButton");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/LineEdit.qml"), uri, 1, 0, "LineEdit");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/SearchEdit.qml"), uri, 1, 0, "SearchEdit");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/MPIcon.qml"), uri, 1, 0, "MPIcon");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/BlurBackground.qml"), uri, 1, 0, "BlurBackground");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/ClipAreaItem.qml"), uri, 1, 0, "ClipAreaItem");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/InnerShadow.qml"), uri, 1, 0, "InnerShadow");
    qmlRegisterType(QUrl(qmlUriPrefix + "/Controls/CustomToolTip.qml"), uri, 1, 0, "CustomToolTip");
}

void MPControlExtension::initializeEngine(QQmlEngine *engine, const char *uri)
{
    engine->addImageProvider("mp.icons", new IconImageProvider());
    engine->imageProvider("mp.icons");

    if (EdgeToEdgeModeHelper *edgeToEdge = engine->singletonInstance<EdgeToEdgeModeHelper *>(qmlTypeId(uri, 1, 0, "EdgeToEdge"))) {
        edgeToEdge->enable();
    }
}
