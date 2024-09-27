import QtQuick
import QtQuick.Controls

import CCMagicPocket

import "title"
import "controls"


Item {
    id: root

    property alias surface: surfaceItem

    Binding {
        when: root.ListView.view !== null
        root.width: root.ListView.view.width
    }

    Binding {
        when: root.ListView.view !== null
        root.height: root.ListView.view.height
    }

    Item {
        id: surfaceItem
        anchors.fill: parent
    }

    StatusBlock {
        z: 100
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        expandingItems: [
            RoundAnimateButton {
                icon.iconName: "minimize_activity"
                icon.iconColor: "white"
                hoveredIn: true
                backgroundColor: Qt.hsla(0.4, 0.7, 0.4, 0.6)
                backgroundBorderColor: Qt.hsla(0.4, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.hide(activityName)
                }
            },

            RoundAnimateButton {
                icon.iconName: "layout_activity"
                icon.iconColor: "white"
                hoveredIn: true
                backgroundColor: Qt.hsla(0.6, 0.7, 0.4, 0.6)
                backgroundBorderColor: Qt.hsla(0.6, 1, 0.2, 0.8)
            },

            RoundAnimateButton {
                icon.iconName: "close_activity"
                icon.iconColor: "white"
                hoveredIn: true
                backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
                backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.stop(activityName)
                }
            }
        ]
    }
}
