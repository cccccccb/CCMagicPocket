import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import CCMagicPocket
import CCMagicPocket.impl

import "effects"
import "activity"

Item {
    id: root
    clip: true

    ActivityContainer {
        id: activityContainer
        visible: false
        anchors.fill: parent
    }

    ActivityLayoutView {
        id: activityLayoutView
        visible: false
        anchors.fill: parent
        model: ModularListModel {
            preModel: ListModel {
                ListElement { activityName: "empty" }
                ListElement { activityName: "empty" }
            }

            midModel: activityManager.runningModel
        }
    }

    ActivityManager {
        id: activityManager
        runningContainer: activityContainer
        runningLayoutView: activityLayoutView
        runningTemplate: Qt.createComponent("activity/ActivityTemplate.qml", root)

        showTransition: activityContainer.showTransition
        hideTransition: activityContainer.hideTransition
        openTransition: activityContainer.openTransition
        closeTransition: activityContainer.closeTransition

        Component.onCompleted: {
            MagicPocket.activityManager = this
            var component = Qt.createComponent("activity/ActivityTemplate.qml", root)
        }
    }
}
