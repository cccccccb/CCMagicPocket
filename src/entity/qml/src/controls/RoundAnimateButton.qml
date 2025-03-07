import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    width: Style.item.extraStyle.packHoverButtonSize
    height: Style.item.extraStyle.packHoverButtonSize

    property color backgroundColor: Style.item.extraStyle.miniButtonBackgroundColor
    property alias backgroundBorderColor: _backRect.border.color
    property alias icon: icon
    property bool hoveredIn: false
    property alias pressedIn: mouseArea.pressed
    signal clicked()

    readonly property bool _hoveredIn: enabled && (hovered || hoveredIn)

    hoverEnabled: true

    state: "UNHOVERED"
    states: [
        State {
            name: "HOVERED"
            when: _hoveredIn
            PropertyChanges {
                target: root
                width: Style.item.extraStyle.packHoverButtonSize
                height: Style.item.extraStyle.packHoverButtonSize
            }

            PropertyChanges {
                target: icon
                scale: 1.0
            }
        },

        State {
            name: "UNHOVERED"
            when: !_hoveredIn
            PropertyChanges {
                target: root
                width: Style.item.extraStyle.packButtonSize
                height: Style.item.extraStyle.packButtonSize
            }

            PropertyChanges {
                target: icon
                scale: 0.0
            }
        }
    ]

    background: Item {
        Rectangle {
            id: _backRect

            anchors.fill: parent
            color: pressedIn ? Qt.darker(root.backgroundColor, 1.2) : hovered ? Qt.lighter(root.backgroundColor, 1.2) : root.backgroundColor
            border.color: Style.item.extraStyle.miniButtonBackgroundBorderColor
            border.width: Style.item.borderWidth
            radius: _backRect.width / 2

            Behavior on color {
                ColorAnimation {
                    duration: 100
                }
            }
        }

        Loader {
            active: _hoveredIn
            anchors.fill: _backRect

            sourceComponent: OuterShadow {
                cornerRadius: _backRect.radius
                shadowColor: Qt.color("#CC0000000")
                shadowRadius: 6
            }
        }
    }

    contentItem: FocusScope {
        id: scope
        MPIcon {
            id: icon
            anchors.centerIn: parent
            pressed: pressedIn
            hovered: root.hovered

            iconWidth: Style.item.miniIconSize
            iconHeight: Style.item.miniIconSize
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onClicked: {
                scope.focus = true
                root.clicked()
            }
        }
    }

    transitions: Transition {
        ParallelAnimation {
            PropertyAnimation {
                targets: root
                properties: "width,height"
                duration: Style.item.animationDuration
                easing.type: Easing.OutBack
            }

            ScaleAnimator {
                target: icon
                duration: Style.item.animationDuration
                easing.type: Easing.OutBack
            }
        }
    }
}
