#ifndef MPPRELOAD_H
#define MPPRELOAD_H

#include <QObject>
#include <QQmlEngine>

#include "interface/appstartuppreloadinterface.h"
#include "model.h"

class MPPreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit MPPreload(QObject *parent = nullptr);
    ~MPPreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadModulePath() const;


    QObject *edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    QPointer<AnimalModel> animalModel;
};

#endif // MPPRELOAD_H
