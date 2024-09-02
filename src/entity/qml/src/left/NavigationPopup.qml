import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Popup {
    id: root

    property alias blurBackground: blurBack.blurBackground

    padding: 0
    implicitWidth: background.implicitWidth

    background: Item { implicitWidth: 160 }

    contentItem: Item {
        clip: true

        MouseArea {
            enabled: root.visible
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            anchors.fill: parent
            onPressedChanged: {
                if (this.pressed)
                    root.close()
            }
        }

        Item {
            id: controls
            x: -160
            y: 10
            width: parent.width - 60
            height: parent.height - 20

            BlurBackground {
                id: blurBack
                visible: false
                anchors.fill: parent
                radius: 12
            }

            MultiEffect {
                source: blurBack
                anchors.fill: blurBack

                shadowEnabled: true
                shadowColor: Qt.color("#F0000000")
                shadowBlur: 0.6
                shadowOpacity: 0.8
            }

            Rectangle {
                id: _blurItem
                anchors.fill: parent

                color: "#40FFFFFF"
                radius: 12
            }

            Item {
                anchors.fill: parent
                WindowButtonPane {
                    id: windowButtonPane

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                }

                LeftNavigationBar {
                    anchors.top: windowButtonPane.bottom
                    anchors.topMargin: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: parent.width - 10
                    height: parent.height - windowButtonPane.height - 10
                }
            }
        }
    }

    enter: Transition {
        SequentialAnimation{
            PauseAnimation { duration: 600 }
            ParallelAnimation {
                NumberAnimation {
                    target: controls
                    properties: "x"
                    duration: 600
                    to: 30
                    easing.type: Easing.OutQuint
                }

                NumberAnimation {
                    target: controls
                    properties: "opacity"
                    duration: 600
                    from: 0.0
                    to: 1.0
                    easing.type: Easing.OutQuint
                }
            }
        }
    }

    exit: Transition {
        ParallelAnimation {
            NumberAnimation {
                target: controls
                properties: "x"
                duration: 600
                to: -controls.width
                easing.type: Easing.InQuint
            }

            NumberAnimation {
                target: controls
                properties: "opacity"
                duration: 600
                from: 1.0
                to: 0.0
                easing.type: Easing.InQuint
            }
        }
    }
}

