import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import CCMagicPocket
import CCMagicPocket.impl

import "effects"
import "activity"

ActivityPlaneItem {
    id: root

    Loader {
        id: activityLayoutViewLoader
        active: root.visible
        anchors.fill: parent

        sourceComponent: ActivityLayoutView {
            model: ModularListModel {
                preModel: ListModel {
                    ListElement { activityName: "empty" }
                    ListElement { activityName: "empty" }
                }

                midModel: MagicPocket.activityManager.runningModel
            }

            function itemAtActivityName(name) {
                return itemAtIndex(model.midModel.indexOf(name) + model.preModel.count)
            }
        }
    }

    popDown: Transition {
        NumberAnimation {
            target: activityLayoutViewLoader
            property: "x"
            to: -activityLayoutViewLoader.width
            duration: 600
            easing.type: Easing.OutQuart
        }
    }
}
