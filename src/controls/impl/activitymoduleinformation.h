#ifndef ACTIVITYMODULEINFORMATION_H
#define ACTIVITYMODULEINFORMATION_H

#include <QQmlComponent>

#include "items/appstartupmoduleinformation.h"

struct ActivityModuleInformation
{
    Q_GADGET
    QML_NAMED_ELEMENT(activityModuleInformation)
    QML_FOREIGN(AppStartupModuleInformation)
    QML_UNCREATABLE("")
};

Q_DECLARE_METATYPE(AppStartupModuleInformation);
#endif // ACTIVITYMODULEINFORMATION_H
