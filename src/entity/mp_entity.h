#ifndef MPENTITY_H
#define MPENTITY_H

#include <QObject>

#include "interface/appstartupentityinterface.h"

class MPQmlInstance;

class MPEntity : public QObject, public AppStartupEntityInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupEntityInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupEntityInterface)

public:
    explicit MPEntity(QObject *parent = nullptr);
    ~MPEntity();

    QUrl entityComponentPath() const;
    void initialize(QQmlApplicationEngine *engine);
};

#endif // MPENTITY_H
