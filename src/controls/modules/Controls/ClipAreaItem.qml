import QtQuick
import QtQuick.Effects

Item {
    id: root
    property Component component
    property real radius: 0

    Loader {
        id: effectSource
        anchors.fill: parent
        visible: false
        active: root.component !== null
        sourceComponent: root.component
    }

    MultiEffect {
        anchors.fill: effectSource
        source: effectSource
        autoPaddingEnabled: false
        maskEnabled: true
        maskSource: mask
        maskThresholdMin: 0.1
        maskSpreadAtMin: 0.1
        maskSpreadAtMax: 1
    }

    Item {
        id: mask
        width: effectSource.width
        height: effectSource.height
        layer.enabled: true
        visible: false
        antialiasing: true

        Rectangle {
            width: effectSource.width
            height: effectSource.height
            radius: root.radius
        }
    }
}
