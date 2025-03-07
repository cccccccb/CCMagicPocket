import QtQuick

import CCMagicPocket

Item {
    z: -100
    property int cornerRadius: 0
    property real shadowRadius: 10
    property real offsetX: 0
    property real offsetY: 0
    property color shadowColor: "black"

    Loader {
        property int _cornerHRadius: Math.min(parent.width / 2, cornerRadius)
        property int _cornerVRadius: Math.min(parent.height / 2, cornerRadius)
        property real _shadowSize: (Math.max(_cornerHRadius, _cornerVRadius) * 2 + shadowRadius * 2 + 1)  // 1 pixel for center

        anchors.centerIn: parent
        anchors.horizontalCenterOffset: offsetX
        anchors.verticalCenterOffset: offsetY

        width: parent.width + ( shadowRadius) * 2
        height: parent.height + (shadowRadius) * 2
        active: parent.width > 0 && parent.height > 0

        sourceComponent: BorderImage {
            border.left: shadowRadius + _cornerHRadius
            border.right: shadowRadius + _cornerHRadius
            border.top: shadowRadius + _cornerVRadius
            border.bottom: shadowRadius + _cornerVRadius

            source: MagicPocket.toMPShadowUrl(_shadowSize, _cornerHRadius, _cornerVRadius, shadowRadius, shadowColor)
        }
    }
}
