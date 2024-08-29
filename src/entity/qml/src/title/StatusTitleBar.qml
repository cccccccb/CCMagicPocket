import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property bool slideOn: false
    property alias backgroundItem: blurBack.backgroundItem

    leftPadding: 10
    rightPadding: 10
    topInset: 10
    bottomInset: 10
    implicitHeight: 44

    background: Item {
        opacity: 0.8

        Rectangle {
            id: _blurItem
            anchors.fill: parent

            visible: false
            color: Qt.lighter("#A0A0A0")
            border.color: "#E0E0E0"
            border.width: 1
            radius: 8
        }

        MultiEffect {
            source: _blurItem
            anchors.fill: _blurItem

            shadowEnabled: true
            shadowColor: Qt.color("#F0000000")
            shadowBlur: 0.6
        }

        BlurBackground {
            id: blurBack
            anchors.fill: parent
            radius: 8
        }
    }

    contentItem: Item {
        Image {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: "qrc:/svg/icon-circle.svg"
            sourceSize: Qt.size(18, 18)
            smooth: true
        }

        Row {
            id: timerItem
            anchors.centerIn: parent
            spacing: 4

            Text {
                font.bold: true
                font.pointSize: Style.item.fontSize.t3
                color: Style.item.hightTextColor

                Timer {
                    repeat: true
                    interval: 500
                    running: true

                    onTriggered: parent.text =  Qt.formatDateTime(new Date(), "MMM-dd")
                }
            }


            Text {
                font.bold: true
                font.pointSize: Style.item.fontSize.t3
                color: Style.item.hightTextColor

                Timer {
                    repeat: true
                    interval: 500
                    running: true

                    onTriggered: parent.text =  Qt.formatDateTime(new Date(), "hh:mm")
                }
            }
        }

        StatusBlock {
            id: statusBlock
            activateOn: slideOn

            width: timerItem.width + 10
            anchors.bottom: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CustomButton {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            display: Button.IconOnly
            icon.name: "notification"
            icon.width: 18
            icon.height: 18
            icon.color: Style.item.hightTextColor
            background: null
        }
    }

    state: "NONSLIDEON"
    states: [
        State {
            name: "SLIDEON"
            when: slideOn
            PropertyChanges {
                target: root
                opacity: 1.0
                y: 10
            }
            PropertyChanges {
                target: statusBlock
                opacity: 1.0
                width: timerItem.width + 10
            }
        },
        State {
            name: "NONSLIDEON"
            when: !slideOn
            PropertyChanges {
                target: root
                opacity: 0.0
                y: -implicitHeight
            }
            PropertyChanges {
                target: statusBlock
                opacity: 0.0
                width:  10
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDEON"
            to: "SLIDEON"

            SequentialAnimation {
                PropertyAnimation {
                    target: root
                    duration: 600
                    properties: "opacity,y"
                    easing.type: Easing.OutQuint
                }

                PauseAnimation {
                    duration: 100
                }

                PropertyAnimation {
                    target: statusBlock
                    duration: 400
                    properties: "opacity,width"
                    easing.type: Easing.OutBack
                }
            }
        },
        Transition {
            from: "SLIDEON"
            to: "NONSLIDEON"

            ParallelAnimation {
                PropertyAnimation {
                    target: root
                    duration: 600
                    properties: "opacity,y"
                    easing.type: Easing.InQuint
                }

                PropertyAnimation {
                    target: statusBlock
                    duration: 400
                    properties: "opacity,width"
                    easing.type: Easing.InBack
                }
            }
        }
    ]
}
