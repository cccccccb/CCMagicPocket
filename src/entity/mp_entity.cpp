#include "mp_entity.h"
#include "appstartupinstance.h"

#include <QIcon>
#include <QQmlApplicationEngine>
#include <QTimer>

MPEntity::MPEntity(QObject *parent)
    : QObject{parent}
{}

MPEntity::~MPEntity()
{

}

QUrl MPEntity::entityModulePath() const
{
    return QUrl("qrc:///CCMagicPocket/qml/src/MainWindow.qml");
}

void MPEntity::initialize(QQmlApplicationEngine *)
{
    QIcon::setThemeName("MagicPocket-desktop");
}
