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

        expandingItems: Item {
            readonly property int controlSpacing: 6
            implicitWidth: minimizeActivityBtn.width + layoutActivityBtn.width + closeActivityBtn.width + controlSpacing * 2
            implicitHeight: 16

            RoundAnimateButton {
                id: minimizeActivityBtn

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                icon.iconName: "minimize_activity"
                icon.iconColor: "white"
                backgroundColor: Qt.hsla(0.4, 0.7, 0.4, 0.6)
                backgroundBorderColor: Qt.hsla(0.4, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.hide(activityName)
                }
            }

            RoundAnimateButton {
                id: layoutActivityBtn

                anchors.leftMargin: parent.controlSpacing
                anchors.left: minimizeActivityBtn.right
                anchors.verticalCenter: parent.verticalCenter

                icon.iconName: "layout_activity"
                icon.iconColor: "white"
                backgroundColor: Qt.hsla(0.6, 0.7, 0.4, 0.6)
                backgroundBorderColor: Qt.hsla(0.6, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.layout(activityName)
                }
            }

            RoundAnimateButton {
                id: closeActivityBtn

                anchors.leftMargin: parent.controlSpacing
                anchors.left: layoutActivityBtn.right
                anchors.verticalCenter: parent.verticalCenter

                icon.iconName: "close_activity"
                icon.iconColor: "white"
                backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
                backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)

                onClicked: {
                    MagicPocket.activityManager.close(activityName)
                }
            }
        }
    }
}
