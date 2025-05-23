import QtQuick
import QtQuick.Controls

import CCMagicPocket
import CCMagicPocket.impl

ListView {
    id: root

    orientation: Qt.Horizontal
    interactive: false
    spacing: 5

    property real slideFactor: 0
    property int interactiveMovingIndex: -1

    delegate: ActivityLayoutDelegate {}
    populate: Transition {
        id: populateTrans
        NumberAnimation {
            properties: "x"
            duration: 600 + populateTrans.ViewTransition.index * 200
            easing.type: Easing.OutBack
            easing.overshoot: 2.4
        }
    }

    PointHandler {
        property real lastPosX: 0.0

        acceptedDevices: PointerDevice.TouchScreen | PointerDevice.TouchPad | PointerDevice.Mouse
        acceptedButtons: Qt.LeftButton
        enabled: root.interactiveMovingIndex === -1

        function releaseSlide() {
            this.lastPosX = 0.0
            root.slideFactor = Math.round(root.slideFactor)
            if (root.slideFactor < -(root.count - 3))
                root.slideFactor = -(root.count - 3)
            else if (root.slideFactor > 4)
                root.slideFactor = 4
        }

        onPointChanged: {
            if (!this.active)
                return

            root.slideFactor += 0.1 * (this.point.position.x - this.lastPosX) / 22
            this.lastPosX = this.point.position.x
        }

        onActiveChanged: {
            if (this.active) {
                this.lastPosX = this.point.pressPosition.x
            } else {
                releaseSlide()
            }
        }
    }

    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: {
            MagicPocket.activityManager.exitLayout("")
        }
    }
}
