import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Item {
    id: root
    property real scaleFactor: 1.0
    property bool slideOn: true
    property bool revertAnimation: false
    property bool isRunningItem: false
    property bool isCurrentItem: isRunningItem ? (MagicPocket.activityManager.currentActivityName === activityName && activityName !== "") : false
    property alias hoverEnabled: _mouseArea.hoverEnabled
    property alias hovered: _mouseArea.containsMouse

    signal leftMouseButtonClicked()
    signal rightMouseButtonClicked()

    implicitWidth: centralControl.implicitWidth + 8
    implicitHeight: 68

    Control {
        id: centralControl
        anchors.centerIn: parent
        hoverEnabled: false
        padding: 6

        transform: Translate {
            y: -2 * (centralControl.height - 48 - centralControl.topPadding - centralControl.bottomPadding)
        }

        background: Rectangle {
            color: "white"
            radius: 12

            Loader {
                active: root.isRunningItem
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.bottom
                anchors.topMargin: 5
                width: root.hovered ? 40 : 20
                height: 3

                sourceComponent: Rectangle {
                    radius: height / 2
                    color: Style.item.hightTextColor
                }

                Behavior on width {
                    SpringAnimation {
                        spring: 4
                        damping: 0.2
                    }
                }
            }

            OuterShadow {
                anchors.fill: parent
                shadowColor: Qt.color("#60000000")
                cornerRadius: parent.radius
                offsetY: 4
            }
        }

        contentItem: Item {
            id: content
            property real _scaleFactorContent: root.scaleFactor

            implicitWidth: 48 * _scaleFactorContent
            implicitHeight: 48 * _scaleFactorContent

            Behavior on _scaleFactorContent {
                SpringAnimation { spring: 4; damping: 0.2 }
            }

            Image {
                id: _blurItem
                anchors.fill: parent

                source: iconSource
                fillMode: Image.PreserveAspectFit
                sourceSize: Qt.size(68, 68)
            }
        }

        Loader {
            active: root.hovered

            sourceComponent: CustomToolTip {
                parent: centralControl
                visible: root.hovered
                text: displayName
            }
        }
    }

    property int _slideOnY: 0

    state: "SLIDE_ON"
    states: [
        State {
            name: "SLIDE_ON"
            when: slideOn
            PropertyChanges {
                target: root
                _slideOnY: 0
            }
        },
        State {
            name: "NONSLIDE_ON"
            when: !slideOn
            PropertyChanges {
                target: root
                _slideOnY: root.height * 2
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDE_ON"
            to: "SLIDE_ON"

            SequentialAnimation {
                PauseAnimation {
                    duration: revertAnimation ? (root.ListView.view.count - 1 - index) * 60 : index * 60
                }

                PropertyAnimation {
                    duration: 800
                    properties: "_slideOnY"
                    easing.type: Easing.OutQuint
                }
            }
        },

        Transition {
            from: "SLIDE_ON"
            to: "NONSLIDE_ON"

            SequentialAnimation {
                PauseAnimation {
                    duration: revertAnimation ? (root.ListView.view.count - 1 - index) * 60 : index * 60
                }

                PropertyAnimation {
                    duration: 400
                    properties: "_slideOnY"
                    easing.type: Easing.InQuint
                }
            }
        }
    ]

    transform: Translate {
        y: _slideOnY
    }

    MouseArea {
        id: _mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: (mouse) => {
            if (mouse.button === Qt.LeftButton)
                root.leftMouseButtonClicked()
            else
                root.rightMouseButtonClicked()
        }
    }
}
