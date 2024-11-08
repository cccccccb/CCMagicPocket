import QtQuick

import "../effects"

Item {
    id: root

    property Item sourceItem

    property var closeTransition: Transition {
        enabled: closeShaderLoader.active

        NumberAnimation {
            target: closeShaderLoader
            properties: "time"
            from: 0.0
            to: 1.0
            duration: 600
        }
    }

    Loader {
        id: closeShaderLoader
        property real time: 0.0
        active: time !== 0.0 && time !== 1.0
        anchors.fill: parent

        sourceComponent: Item {
            ShaderEffectSource {
                id: sourceEffect
                anchors.fill: parent
                sourceItem: root.sourceItem
                hideSource: true
                visible: false
            }

            BurnOutEffect {
                anchors.fill: sourceEffect
                source: sourceEffect
                time: closeShaderLoader.time
            }
        }
    }
}
