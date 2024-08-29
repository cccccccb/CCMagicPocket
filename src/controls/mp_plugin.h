#ifndef MP_PLUGIN_H
#define MP_PLUGIN_H

#include <QQmlExtensionPlugin>

class MPQmlInstance;
class EdgeToEdgeModeHelper;
class QJSEngine;

class MPControlExtension : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    explicit MPControlExtension(QObject *parent = nullptr);
    ~MPControlExtension() override;;

    void registerTypes(const char *uri) override;
    void initializeEngine(QQmlEngine *engine, const char *uri) override;

    QObject *edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
    QObject *mp_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    QPointer<EdgeToEdgeModeHelper> edgeToEdgeHelper;
    QPointer<MPQmlInstance> mpInstance;
};

#endif // MP_PLUGIN_H
