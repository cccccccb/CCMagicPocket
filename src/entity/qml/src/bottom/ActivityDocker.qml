import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property bool slideOn: false
    property alias blurBackground: blurBack.blurBackground
    property alias permanentModel: permanentView.model
    property alias activeModel: activeView.model

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + topPadding

    padding: 10
    background: Item {
        opacity: 0.8

        Rectangle {
            id: _blurItem
            anchors.fill: parent

            color: Qt.lighter("#A0A0A0")
            border.color: "#E0E0E0"
            border.width: 1
            radius: 12

            OuterShadow {
                anchors.fill: parent
                cornerRadius: _blurItem.radius
                shadowColor: Qt.color("#80000000")
                shadowRadius: 20
            }
        }

        BlurBackground {
            id: blurBack
            anchors.fill: parent
            radius: 12
        }
    }

    contentItem: Row {
        spacing: 10

        CustomButton {
            anchors.verticalCenter: parent.verticalCenter
            display: CustomButton.IconOnly
            icon.name: "all_application"
            icon.width: 48
            icon.height: 48
            icon.color: "white"
            width: 48
            height: 48
            background: null
        }

        Loader {
            active: permanentView.count > 0
            visible: permanentView.count > 0
            anchors.verticalCenter: parent.verticalCenter
            width: 2
            height: Math.max(permanentView.height, activeView.height)

            sourceComponent: Rectangle {
                width: 2
                radius: 1
                color: Qt.alpha(Style.item.hightTextColor, 0.6)
            }
        }

        ActivityDockerView {
            id: permanentView
            slideOn: root.slideOn
            model: MagicPocket.activityManager.installModel

            Component.onCompleted: {
                MagicPocket.activityManager.install("C:/workspace/project/CCMagicPocket/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/mpk/CCMagicPocketExample.mpk")
            }
        }

        Loader {
            active: activeView.count > 0
            visible: activeView.count > 0
            anchors.verticalCenter: parent.verticalCenter
            width: 2
            height: Math.max(permanentView.height, activeView.height)

            sourceComponent: Rectangle {
                width: 2
                radius: 1
                color: Qt.alpha(Style.item.hightTextColor, 0.6)
            }
        }

        ActivityDockerView {
            id: activeView
            slideOn: root.slideOn
            revertAnimation: true
            runningActivity: true
            model: MagicPocket.activityManager.runningModel

            Component.onCompleted: {
                MagicPocket.activityManager.dockerContainer = activeView
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
                y: parent.height - height - 20
            }
        },
        State {
            name: "NONSLIDEON"
            when: !slideOn
            PropertyChanges {
                target: root
                opacity: 0.0
                y: parent.height
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDEON"
            to: "SLIDEON"

            PropertyAnimation {
                target: root
                duration: 600
                properties: "opacity,y"
                easing.type: Easing.OutQuint
            }
        },
        Transition {
            from: "SLIDEON"
            to: "NONSLIDEON"

            PropertyAnimation {
                target: root
                duration: 600
                properties: "opacity,y"
                easing.type: Easing.InQuint
            }
        }
    ]
}
