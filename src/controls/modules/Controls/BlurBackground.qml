import QtQuick
import QtQuick.Effects

import "../Utils/functional.js" as Functional

Item {
    id: root
    property Item blurBackground
    property real radius: 0

    function scheduleUpdateOnInterval() {
        var rect = Functional.mapToTarget(root, blurBackground, Qt.rect(root.x, root.y, root.width, root.height))

        effectSource.sourceRect = rect
        effectSource.scheduleUpdate()
    }

    Instantiator {
        readonly property Item _parentBackground: Functional.commonParent(root, blurBackground)
        active:  _parentBackground !== null
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
        sourceItem: blurBackground
        recursive: false
        live: true
        visible: false
    }

    MultiEffect {
        anchors.fill: effectSource
        source: effectSource
        autoPaddingEnabled: false
        blurEnabled: true
        blurMultiplier: 0.4
        blurMax: 48
        blur: 0.8
        maskEnabled: root.radius > 0
        maskSource: mask
        maskThresholdMin: 0.1
        maskSpreadAtMin: 0.1
        maskSpreadAtMax: 1
    }

    Rectangle {
        id: mask

        layer.enabled: root.radius > 0
        visible: false
        antialiasing: true
        width: effectSource.width
        height: effectSource.height
        radius: root.radius
    }
}
