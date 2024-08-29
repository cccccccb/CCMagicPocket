import QtQuick
import QtQuick.Effects

import "../Utils/functional.js" as Functional

Item {
    id: root
    property Item backgroundItem
    property real radius: 0

    function scheduleUpdateOnInterval() {
        if (scheduleUpdateTimer.running)
            return

        scheduleUpdateTimer.start()
    }

    Instantiator {
        readonly property Item _parentBackground: Functional.commonParent(root, backgroundItem)
        model: Functional.parentsTo(root, _parentBackground)

        Connections {
            target: modelData

            function onXChanged() {
                scheduleUpdateOnInterval()
            }

            function onYChanged() {
                scheduleUpdateOnInterval()
            }

            function onWidthChanged() {
                scheduleUpdateOnInterval()
            }

            function onHeightChanged() {
                scheduleUpdateOnInterval()
            }
        }
    }

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: backgroundItem
        recursive: false
        live: false
        visible: false
    }

    MultiEffect {
        anchors.fill: effectSource
        source: effectSource
        autoPaddingEnabled: false
        blurEnabled: true
        blurMax: 64
        blur: 1
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

    Timer {
        id: scheduleUpdateTimer
        interval: 5
        repeat: false

        onTriggered: {
            var rect = Functional.mapToTarget(root, backgroundItem, Qt.rect(root.x, root.y, root.width, root.height))
            if (rect.x < 0)
                rect.x = 0

            if (rect.y < 0)
                rect.y = 0

            effectSource.sourceRect = rect
            effectSource.scheduleUpdate()
        }
    }
}
