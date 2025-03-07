import QtQuick
import QtQuick.Effects
import QtQuick.Layouts

import CCMagicPocket

Item {
    id: root

    property real motionFactor: index + ListView.view.slideFactor
    property real motionFunc: motionFactor >= -0.5 ? ((4 * motionFactor + 2) / (2 * motionFactor + 13)) : 0
    property real calcWidth: (index === (ListView.view.count - 1)) ? contentLoader.width : motionFunc * (ListView.view.width - contentLoader.width)
    property bool pressAndHold: false

    width: calcWidth > 0 ? calcWidth : 0
    height: ListView.view.height

    opacity: motionFunc > 0 ? 1.0 : 0
    visible: opacity > 0
    z: pressAndHold ? root.ListView.view.count + 1 : index

    MotionBehavior on calcWidth { duration: 600 + (root.motionFactor > 0 ? root.motionFactor * 200 : 0) }
    Behavior on opacity { NumberAnimation { duration: 600 + (root.motionFactor > 0 ? root.motionFactor * 200 : 0) } }

    Loader {
        id: contentLoader
        property real contentYTranslate: 0.0

        active: index > 1 && MagicPocket.activityManager.runningLayoutView.visible
        width: root.ListView.view.width * 0.8
        height: root.ListView.view.height * 0.8
        anchors.left: parent.left
        y: (parent.height - height) / 2 + contentLoader.contentYTranslate

        NumberAnimation {
            id: contentYMotionAnimation
            target: contentLoader
            properties: "contentYTranslate"
            easing.type: Easing.OutBack
            duration: 600
        }

        sourceComponent: Item {
            id: contentItem
            readonly property Item visibleItem: MagicPocket.activityManager.runningItemAt(activityName)
            readonly property real scaleFactor: 0.85 + root.motionFactor * 0.025

            scale: (pressAndHold ? 1.05 : 1) * (scaleFactor > 1.0 ? 1.0 : scaleFactor)
            transform: Translate { x: (contentItem.scale - 1) / 2 * contentItem.width }

            MotionBehavior on scale {
                duration: pressAndHold ? 400 : (600 + (root.motionFactor > 0 ? root.motionFactor * 200 : 0))
            }

            RowLayout {
                id: acIdLayout
                anchors.bottom: sourceEffect.top
                anchors.bottomMargin: 10
                anchors.left: sourceEffect.left
                anchors.leftMargin: 20
                spacing: 6

                Image {
                    opacity: root.calcWidth > this.width ? 1.0 : 0.0
                    source: iconSource
                    fillMode: Image.PreserveAspectFit
                    sourceSize: Qt.size(32, 32)

                    MotionBehavior on opacity {
                        duration: 400
                    }
                }

                Text {
                    opacity: root.calcWidth > acIdLayout.width ? 1.0 : 0.0
                    text: displayName
                    font.pointSize: 10
                    font.bold: true
                    color: Style.item.backgroundTitleColor
                    verticalAlignment: Text.AlignVCenter

                    MotionBehavior on opacity {
                        duration: 400
                    }
                }
            }

            Rectangle {
                id: sourceEffect
                anchors.fill: parent

                color: "white"
                radius: 12
                visible: false

                Loader {
                    anchors.fill: parent
                    active: visibleItem !== null

                    sourceComponent: ShaderEffectSource {
                        sourceItem: visibleItem
                        hideSource: true
                        live: false
                    }
                }

                OuterShadow {
                    shadowColor: Qt.color("#40000000")
                    shadowRadius: 20
                    anchors.fill: sourceEffect
                    cornerRadius: sourceEffect.radius
                }
            }
        }
    }

    PointHandler {
        property int direction: -1
        property real lastPosY: 0.0

        acceptedDevices: PointerDevice.TouchScreen | PointerDevice.TouchPad | PointerDevice.Mouse
        acceptedButtons: Qt.LeftButton
        enabled: contentLoader.active && (root.ListView.view.interactiveMovingIndex === -1 || root.ListView.view.interactiveMovingIndex === index)

        onPointChanged: {
            if (!this.active)
                return

            if (this.direction === -1) {
                var xMovedAbs = Math.abs(this.point.position.x - this.point.pressPosition.x);
                var yMovedAbs = Math.abs(this.point.position.y - this.point.pressPosition.y)

                if (xMovedAbs < 5 && yMovedAbs < 5)
                    return

                if ((xMovedAbs * 3) > yMovedAbs) {
                    this.direction = Qt.Horizontal
                } else {
                    this.direction = Qt.Vertical
                }
            }

            if (this.direction === Qt.Horizontal)
                return

            if (root.ListView.view.interactiveMovingIndex !== index)
                root.ListView.view.interactiveMovingIndex = index

            var yMovedStep = this.point.position.y - this.lastPosY

            if (yMovedStep > 0)
                contentLoader.contentYTranslate += 0.4 * yMovedStep
            else
                contentLoader.contentYTranslate += yMovedStep

            this.lastPosY = this.point.position.y
        }

        onActiveChanged: {
            if (!this.active) {
                this.lastPosY = 0.0
                contentYMotionAnimation.to = 0.0
                contentYMotionAnimation.start()
                this.direction = -1
                root.ListView.view.interactiveMovingIndex = -1
            } else {
                if (contentYMotionAnimation.running) {
                    contentYMotionAnimation.stop()
                }

                this.lastPosY = this.point.pressPosition.y
            }
        }
    }
}
