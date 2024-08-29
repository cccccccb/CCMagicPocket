import QtQuick
import QtQuick.Effects

Item {
    id: root
    property color shadowColor: Qt.color("#FF000000")
    property real radius
    property Component component

    Loader {
        id: effectSource
        anchors.fill: parent
        visible: false
        active: root.component !== null
        sourceComponent: root.component
    }

    MultiEffect {
        source: effectSource
        anchors.fill: effectSource

        shadowEnabled: true
        shadowColor: root.shadowColor
        shadowBlur: 1
        autoPaddingEnabled: false
        maskEnabled: true
        maskSource: mask
        maskThresholdMin: 0.6
        maskSpreadAtMin: 0.6
        maskSpreadAtMax: 1
    }

    Item {
        id: mask
        width: effectSource.width
        height: effectSource.height
        layer.enabled: true
        visible: false

        Rectangle {
            width: effectSource.width
            height: effectSource.height
            radius: root.radius
        }
    }
}
