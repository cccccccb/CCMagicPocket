#ifndef TOKLIVEENTITY_H
#define TOKLIVEENTITY_H

#include <QObject>
#include <QQmlEngine>

#include "interface/appstartupentityinterface.h"

class TokLiveQmlInstance;

class TokLiveEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit TokLiveEntity(QObject *parent = nullptr);
    ~TokLiveEntity();

    QUrl entityModulePath() const;
    void initialize(QQmlApplicationEngine *engine);

    QObject *edge_to_edge_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // TOKLIVEENTITY_H
