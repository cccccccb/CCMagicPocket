import QtQuick

import CCMagicPocket

Item {
    property int cornerRadius: 0
    property real shadowRadius: 10
    property real offsetX: 0
    property real offsetY: 0
    property color shadowColor: "black"
    property bool surround: true
    property bool inner: false

    Loader {
        readonly property int _cornerHRadius: Math.min(parent.width / 2, cornerRadius)
        readonly property int _cornerVRadius: Math.min(parent.height / 2, cornerRadius)
        readonly property real _shadowSize: (Math.max(_cornerHRadius, _cornerVRadius) * 2 + shadowRadius * 2 + 1)  // 1 pixel for center
        readonly property real _offsetX: offsetX > shadowRadius ? shadowRadius : offsetX
        readonly property real _offsetY: offsetY > shadowRadius ? shadowRadius : offsetY

        anchors.centerIn: parent
        anchors.horizontalCenterOffset: _offsetX
        anchors.verticalCenterOffset: _offsetY

        width: inner ? parent.width : parent.width + shadowRadius * 2
        height: inner ? parent.height : parent.height + shadowRadius * 2
        active: parent.width > 0 && parent.height > 0

        sourceComponent: BorderImage {
            border.left: shadowRadius + _cornerHRadius
            border.right: shadowRadius + _cornerHRadius
            border.top: shadowRadius + _cornerHRadius
            border.bottom: shadowRadius + _cornerHRadius

            source: MagicPocket.toMPShadowUrl(_shadowSize, _cornerHRadius, _cornerVRadius, shadowRadius,
                                              shadowColor, surround, _offsetX, _offsetY, inner)
        }
    }
}
