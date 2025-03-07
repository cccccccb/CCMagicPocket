import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root

    hoverEnabled: true
    z: hoveredIn ? 5 : 0
    readonly property bool hoveredIn: enabled ? hovered && hoverTimer.running : false
    property color backgroundColor: Qt.rgba(1, 1, 1, 0.7)

    background: Loader {
        active: hoveredIn

        sourceComponent: Item {
            Rectangle {
                id: _backRect
                anchors.fill: parent
                color: root.backgroundColor
                radius: 8
                border.color: Qt.rgba(0, 0, 0, 0.2)
                border.width: Style.item.borderWidth

                OuterShadow {
                    anchors.fill: parent
                    cornerRadius: _backRect.radius
                    shadowColor: Qt.rgba(1, 1, 1, 0.4)
                    shadowRadius: 20
                }
            }
        }
    }

    Timer {
        id: hoverTimer
        interval: 1500
        running: root.hovered
        repeat: false
    }
}
