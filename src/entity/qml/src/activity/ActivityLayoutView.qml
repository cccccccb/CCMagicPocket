import QtQuick
import QtQuick.Controls

import CCMagicPocket

ListView {
    id: root

    orientation: Qt.Horizontal
    interactive: false

    property real slideFactor: 0
    property int interactiveMovingIndex: -1

    delegate: ActivityLayoutDelegate {}

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
            else if (root.slideFactor > 0)
                root.slideFactor = 0
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
