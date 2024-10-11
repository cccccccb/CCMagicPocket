import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import CCMagicPocket
import CCMagicPocket.impl


Item {
    id: root

    clip: true

    Item {
        id: activityView
        property bool delayHide: false

        visible: false
        anchors.fill: parent

        state: "NO_VISIBLE"
        states: [
            State {
                name: "NO_VISIBLE"
                when: !activityView.visible && !activityView.delayHide
                PropertyChanges {
                    target: invertShaderItem
                    bend: 1
                    minimize: 1
                }
            },

            State {
                name: "DELAY_HIDE"
                when: activityView.visible && activityView.delayHide
                PropertyChanges {
                    target: invertShaderItem
                    bend: 1
                    minimize: 1
                }
            },

            State {
                name: "VISIBLE"
                when: activityView.visible && !activityView.delayHide
                PropertyChanges {
                    target: invertShaderItem
                    bend: 0
                    minimize: 0
                }
            }
        ]

        transitions: [
            Transition {
                from: "NO_VISIBLE"
                to: "VISIBLE"

                ParallelAnimation {
                    SequentialAnimation {
                        PauseAnimation {
                            duration: 300
                        }

                        NumberAnimation {
                            target: invertShaderItem
                            properties: "bend"
                            duration: 700
                            easing.type: Easing.InOutSine
                        }
                    }

                    SequentialAnimation {
                        NumberAnimation {
                            target: invertShaderItem
                            properties: "minimize"
                            duration: 700
                            easing.type: Easing.InOutSine
                        }

                        PauseAnimation {
                            duration: 300
                        }
                    }
                }
            },

            Transition {
                from: "VISIBLE"
                to: "DELAY_HIDE"

                SequentialAnimation {
                    PropertyAction { target: activityView; property: "delayHide"; value: true }

                    ParallelAnimation {
                        SequentialAnimation {
                            PauseAnimation {
                                duration: 300
                            }

                            NumberAnimation {
                                target: invertShaderItem
                                properties: "minimize"
                                duration: 700
                                easing.type: Easing.InOutSine
                            }
                        }

                        SequentialAnimation {
                            NumberAnimation {
                                target: invertShaderItem
                                properties: "bend"
                                duration: 700
                                easing.type: Easing.InOutSine
                            }

                            PauseAnimation {
                                duration: 300
                            }
                        }
                    }

                    PropertyAction { target: activityView; property: "delayHide"; value: false }
                 }
            }
        ]

        ListView {
            id: templateLV
            anchors.fill: parent
            orientation: Qt.Horizontal
            visible: invertShaderItem.bend === 0 && invertShaderItem.minimize === 0
            model: ObjectModel {
                children: MagicPocket.activityManager.runningActivity
            }
        }

        ShaderEffectSource {
            id: sourceEffect
            sourceItem: templateLV
        }

        ShaderEffect {
            id: invertShaderItem
            visible: !templateLV.visible
            anchors.fill: parent
            property variant source: sourceEffect
            property real bend
            property real minimize
            property real side: 0.8
            mesh: Qt.size(20, 20)
            vertexShader: "shadereffects/genie.vert.qsb"
        }
    }

    ActivityManager {
        runningContainer: activityView
        runningTemplate: Qt.createComponent("ActivityTemplate.qml", root)

        Component.onCompleted: {
            MagicPocket.activityManager = this
        }
    }
}
