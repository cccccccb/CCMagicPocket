import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property bool slideOn: false
    property alias blurBackground: blurBack.blurBackground
    property int blockSpaceHeight: 24

    leftPadding: 10
    rightPadding: 10
    bottomPadding: 10
    topInset: blockSpaceHeight
    bottomInset: 10

    background: Item {
        opacity: 0.8

        Rectangle {
            id: _blurItem
            anchors.fill: parent

            color: Qt.lighter("#A0A0A0")
            border.color: "#E0E0E0"
            border.width: 1
            radius: 8

            OuterShadow {
                anchors.fill: parent
                cornerRadius: _blurItem.radius
                shadowColor: Qt.color("#F0000000")
                shadowRadius: 20
            }
        }

        BlurBackground {
            id: blurBack
            anchors.fill: parent
            radius: 8
        }

        Component.onCompleted: {
            Window.window.Frameless.moveUnder.push(this)
        }
    }

    contentItem: Item {
        implicitHeight: root.blockSpaceHeight + realTitleItem.implicitHeight

        StatusBlock {
            id: statusBlock
            z: 5

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            expandingItems: WindowButtonPane { }
        }

        Item {
            id: realTitleItem
            width: parent.width
            implicitHeight: 24
            anchors.top: parent.top
            anchors.topMargin: root.blockSpaceHeight
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                source: "../../../res/svg/icon-circle.svg"
                sourceSize: Qt.size(18, 18)
                smooth: true
            }

            Row {
                id: timerItem
                anchors.centerIn: parent
                spacing: 6

                Text {
                    id: dayText
                    font.bold: true
                    font.pointSize: Style.item.fontSize.t3
                    color: Style.item.hightTextColor
                }

                Text {
                    id: timeText
                    font.bold: true
                    font.pointSize: Style.item.fontSize.t3
                    color: Style.item.hightTextColor
                }

                function updateDateTime() {
                    var date = new Date()

                    dayText.text = Qt.formatDateTime(date, "MMM-dd")
                    timeText.text = Qt.formatDateTime(date, "hh:mm")

                    dateTimer.interval = 60 - date.getSeconds()
                    dateTimer.start()
                }

                Timer {
                    id: dateTimer
                    repeat: false

                    onTriggered: parent.updateDateTime()
                }

                Component.onCompleted: updateDateTime()
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

                Component.onCompleted: {
                    Window.window.Frameless.moveExclude.push(this)
                }
            }
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
                y: 0
            }
            PropertyChanges {
                target: statusBlock
                opacity: 1.0
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
                    properties: "opacity"
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
                    properties: "opacity"
                    easing.type: Easing.InBack
                }
            }
        }
    ]
}
