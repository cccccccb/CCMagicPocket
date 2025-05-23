import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket
import CCMagicPocket.impl

import "../title"
import "../controls"
import "../effects"

ActivityTemplateItem {
    id: root

    width: parent.width
    height: parent.height

    signal minizedTransitionFinished
    signal showTransitionStarted

    Item {
        id: templateSurface
        anchors.fill: parent

        StatusBlock {
            z: 100
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            blurBackground: root.surface

            expandingItems: Item {
                readonly property int controlSpacing: 6
                implicitWidth: minimizeActivityBtn.width + layoutActivityBtn.width + closeActivityBtn.width + controlSpacing * 2
                implicitHeight: 16

                RoundAnimateButton {
                    id: minimizeActivityBtn

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    icon.iconName: "minimize_activity"
                    icon.iconColor: "white"
                    backgroundColor: Qt.hsla(0.4, 0.7, 0.4, 0.6)
                    backgroundBorderColor: Qt.hsla(0.4, 1, 0.2, 0.8)
                    onClicked: root.minimized(root.activityName)
                }

                RoundAnimateButton {
                    id: layoutActivityBtn

                    anchors.leftMargin: parent.controlSpacing
                    anchors.left: minimizeActivityBtn.right
                    anchors.verticalCenter: parent.verticalCenter

                    icon.iconName: "layout_activity"
                    icon.iconColor: "white"
                    backgroundColor: Qt.hsla(0.6, 0.7, 0.4, 0.6)
                    backgroundBorderColor: Qt.hsla(0.6, 1, 0.2, 0.8)
                    onClicked: root.layouted(root.activityName)
                }

                RoundAnimateButton {
                    id: closeActivityBtn

                    anchors.leftMargin: parent.controlSpacing
                    anchors.left: layoutActivityBtn.right
                    anchors.verticalCenter: parent.verticalCenter

                    icon.iconName: "close_activity"
                    icon.iconColor: "white"
                    backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
                    backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)
                    onClicked: root.closed(root.activityName)
                }
            }
        }
    }

    surface: templateSurface
    openTrans: Transition {
        ScaleAnimator {
            target: root
            from: 0.0
            to: 1.0
            duration: 500
            easing.type: Easing.OutCubic
        }
    }

    Loader {
        id: sourceLoader
        active: closeShaderLoader.active || invertShaderLoader.active

        sourceComponent: ShaderEffectSource {
            sourceItem: root.surface
            hideSource: true
            visible: false
            live: false
        }
    }

    Loader {
        id: closeShaderLoader

        property real time: 0.0
        active: false
        anchors.fill: parent

        sourceComponent: BurnOutEffect {
            source: sourceLoader.item
            time: closeShaderLoader.time
        }
    }

    Loader {
        id: invertShaderLoader
        property real bend: 0.0
        property real minimize: 0.0
        property real side: 0.8
        active: false
        anchors.fill: parent

        sourceComponent: MagicInEffect {
            source: sourceLoader.item
            bend: invertShaderLoader.bend
            minimize: invertShaderLoader.minimize
            side: invertShaderLoader.side
        }
    }

    function getCurrentDockerSide()
    {
        var item = MagicPocket.activityManager.dockerContainer.runningActivityItem
        if (item === null || item === undefined)
            return 0.8

        return root.mapFromItem(item.parent, item.x + item.width / 2, 0).x / root.width
    }

    // closeTrans: Transition {
    //     enabled: root.visible

    //     SequentialAnimation {
    //         PropertyAction {
    //             target: closeShaderLoader
    //             property: "active"
    //             value: true
    //         }

    //         NumberAnimation {
    //             target: closeShaderLoader
    //             properties: "time"
    //             from: 0.0
    //             to: 1.0
    //             duration: 500
    //         }

    //         PropertyAction {
    //             target: closeShaderLoader
    //             property: "active"
    //             value: false
    //         }
    //     }
    // }

    minimizedTrans: Transition {
        SequentialAnimation {
            PropertyAction {
                target: invertShaderLoader
                property: "active"
                value: true
            }

            PropertyAction {
                target: invertShaderLoader
                properties: "side"
                value: getCurrentDockerSide()
            }

            ParallelAnimation {
                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        property: "minimize"
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
                        property: "bend"
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

            PropertyAction {
                target: invertShaderLoader
                property: "active"
                value: false
            }

            ScriptAction {
                script: root.minizedTransitionFinished()
            }
        }
    }

    showTrans: Transition {
        SequentialAnimation {
            ScriptAction {
                script: root.showTransitionStarted()
            }

            PropertyAction {
                target: invertShaderLoader
                property: "side"
                value: getCurrentDockerSide()
            }

            PropertyAction {
                target: invertShaderLoader
                property: "active"
                value: true
            }

            ParallelAnimation {
                SequentialAnimation {
                    PropertyAction {
                        target: invertShaderLoader
                        property: "bend"
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
                        property: "minimize"
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

            PropertyAction {
                target: invertShaderLoader
                property: "active"
                value: false
            }
        }
    }

    // layoutTrans: Transition {
    //     ParentAnimation {
    //         target: templateSurface
    //         newParent: sourceContainer

    //         NumberAnimation {
    //             target: templateSurface
    //             properties: "x,y"
    //             to: 0
    //             duration: 400
    //             easing.type: Easing.OutQuart
    //         }
    //     }
    // }

    // exitLayoutTrans: Transition {
    //     ParentAnimation {
    //         target: templateSurface
    //         newParent: root

    //         NumberAnimation {
    //             target: templateSurface
    //             properties: "x,y"
    //             to: 0
    //             duration: 400
    //             easing.type: Easing.OutQuart
    //         }
    //     }
    // }
}
