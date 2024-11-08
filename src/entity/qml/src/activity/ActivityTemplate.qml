import QtQuick
import QtQuick.Controls

import CCMagicPocket

import "../title"
import "../controls"


Item {
    id: root

    property bool isCurrentItem: activityName === MagicPocket.activityManager.runningActivity && activityName !== ""
    property Item surface: Item {
        parent: root
        anchors.fill: parent
    }

    width: MagicPocket.activityManager.runningContainer.width
    height: MagicPocket.activityManager.runningContainer.height

    StatusBlock {
        z: 100
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        blurBackground: root.surface

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

                onClicked: {
                    MagicPocket.activityManager.layout(activityName)
                }
            },

            RoundAnimateButton {
                icon.iconName: "close_activity"
                icon.iconColor: "white"
                hoveredIn: true
                backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
                backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.close(activityName)
                }
            }
        ]
    }
}
