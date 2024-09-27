import QtQuick
import QtQuick.Effects

Item {
    id: root
    property Component component
    property real radius: 0
    property alias clipItem: effectSource.item

    Loader {
        id: effectSource
        anchors.fill: parent
        visible: false
        clip: true
        active: root.component !== null
        sourceComponent: root.component
    }

    MultiEffect {
        anchors.fill: effectSource
        source: effectSource
        maskEnabled: true
        maskSource: mask
        maskThresholdMin: 0.1
        maskSpreadAtMin: 0.1
        maskSpreadAtMax: 1
    }

    Item {
        id: mask
        layer.enabled: true
        width: effectSource.width
        height: effectSource.height
        visible: false

        Rectangle {
            width: effectSource.width
            height: effectSource.height
            radius: root.radius
        }
    }
}
