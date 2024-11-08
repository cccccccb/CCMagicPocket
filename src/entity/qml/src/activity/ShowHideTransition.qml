import QtQuick

import CCMagicPocket

import "../effects"

Item {
    id: root

    property Item sourceItem

    property var showTransition: Transition {
        SequentialAnimation {
            PropertyAction {
                target: invertShaderLoader
                properties: "side"
                value: invertShaderLoader.active ? getCurrentDockerSide() : 0.0
            }

            ParallelAnimation {
                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        properties: "bend"
                        value: 1.0
                    }

                    PauseAnimation {
                        duration: 200
                    }

                    NumberAnimation {
                        target: invertShaderLoader
                        properties: "bend"
                        to: 0.0
                        duration: 600
                        easing.type: Easing.InOutSine
                    }
                }

                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        properties: "minimize"
                        value: 1.0
                    }

                    NumberAnimation {
                        target: invertShaderLoader
                        properties: "minimize"
                        to: 0
                        duration: 600
                        easing.type: Easing.InOutSine
                    }

                    PauseAnimation {
                        duration: 200
                    }
                }
            }
        }
    }

    property var hideTransition: Transition {
        SequentialAnimation {
            PropertyAction {
                target: invertShaderLoader
                properties: "side"
                value: getCurrentDockerSide()
            }

            ParallelAnimation {
                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        properties: "minimize"
                        value: 0.0
                    }

                    PauseAnimation {
                        duration: 200
                    }

                    NumberAnimation {
                        target: invertShaderLoader
                        properties: "minimize"
                        to: 1.0
                        duration: 600
                        easing.type: Easing.InOutSine
                    }
                }

                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        properties: "bend"
                        value: 0.0
                    }

                    NumberAnimation {
                        target: invertShaderLoader
                        properties: "bend"
                        to: 1.0
                        duration: 600
                        easing.type: Easing.InOutSine
                    }

                    PauseAnimation {
                        duration: 200
                    }
                }
            }
        }
    }

    Loader {
        id: invertShaderLoader
        property real bend: 0.0
        property real minimize: 0.0
        property real side: 0.8
        active: bend !== 0.0 || minimize !== 0.0
        anchors.fill: parent

        sourceComponent: Item {
            ShaderEffectSource {
                id: sourceEffect
                anchors.fill: parent
                sourceItem: root.sourceItem
                hideSource: true
                visible: false
            }

            MagicInEffect {
                source: sourceEffect
                anchors.fill: sourceEffect
                bend: invertShaderLoader.bend
                minimize: invertShaderLoader.minimize
                side: invertShaderLoader.side
            }
        }
    }

    function getCurrentDockerSide()
    {
        var item = MagicPocket.activityManager.dockerContainer.runningActivityItem
        if (item === null || item === undefined)
            return 0.8

        return root.mapFromItem(item.parent, item.x + item.width / 2, 0).x / root.width
    }
}
