#ifndef MPPRELOAD_H
#define MPPRELOAD_H

#include <QObject>

#include "interface/appstartuppreloadinterface.h"

class MPPreload : public QObject, public AppStartupPreloadInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID AppStartupPreloadInterface_iid FILE "plugin.json")
    Q_INTERFACES(AppStartupPreloadInterface)

public:
    explicit MPPreload(QObject *parent = nullptr);
    ~MPPreload();

    void aboutToPreload(QQmlApplicationEngine *engine);
    QUrl preloadComponentPath() const;
};

#endif // MPPRELOAD_H
