import QtQml
import QtQuick
import QtQuick.Effects
import QtQuick.Controls
import QtQuick.Controls.Material

import CCMagicPocket
import CCStartup

CustomButton {
    id: root

    property bool slideOn: false
    property alias blurBackground: blurBack.blurBackground

    implicitWidth: 40
    implicitHeight: 40
    x: 0
    anchors.verticalCenter: parent.verticalCenter

    display: Button.IconOnly
    icon.name: "slide-out"
    Material.elevation: 0
    Material.foreground: Qt.lighter(Material.accentColor)

    state: "NONSLIDEON"
    states: [
        State {
            name: "SLIDEON"
            when: slideOn

            PropertyChanges {
                target: root
                opacity: 1.0
                x: 10
            }
        },
        State {
            name: "NONSLIDEON"
            when: !slideOn
            PropertyChanges {
                target: root
                opacity: 0.0
                x: 10 - implicitWidth
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDEON"
            to: "SLIDEON"

            SequentialAnimation {
                PauseAnimation { duration: 400 }
                PropertyAnimation {
                    target: root
                    properties: "opacity,x"
                    easing.type: Easing.OutQuint
                }
            }
        },
        Transition {
            from: "SLIDEON"
            to: "NONSLIDEON"

            PropertyAnimation {
                target: root
                properties: "opacity,x"
                easing.type: Easing.InQuint
            }
        }
    ]

    background: Item {
        opacity: 0.8

        Rectangle {
            id: _blurItem
            anchors.fill: parent

            visible: false
            color: Qt.lighter("#A0A0A0A0")
            border.color: "#A0F0F0F0"
            border.width: 1
            radius: root.width / 2
        }

        MultiEffect {
            source: _blurItem
            anchors.fill: _blurItem

            shadowEnabled: true
            shadowColor: Qt.color("#AA000000")
            shadowBlur: 0.6
        }

        BlurBackground {
            id: blurBack
            anchors.fill: parent
            radius: root.width / 2
        }
    }
}
