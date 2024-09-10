#include "toklive_entity.h"
#include "appstartupinstance.h"

#include <QIcon>
#include <QQmlApplicationEngine>

TokLiveEntity::TokLiveEntity(QObject *parent)
    : QObject{parent}
{}

TokLiveEntity::~TokLiveEntity()
{

}

QUrl TokLiveEntity::entityModulePath() const
{
    return QUrl("qrc:///TokLive/src/MainWindow.qml");
}

void TokLiveEntity::initialize(QQmlApplicationEngine *)
{
    // QIcon::setThemeName("TokLive-desktop");
}
