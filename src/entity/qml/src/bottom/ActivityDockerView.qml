import QtQuick

import CCMagicPocket

ListView {
    id: root
    property int hoveredIndex: -1
    property bool slideOn: false
    property bool revertAnimation: false
    property bool runningActivity: false

    interactive: false
    implicitWidth: contentItem.childrenRect.width
    implicitHeight: 68
    orientation: Qt.Horizontal

    delegate: ActivityDockerItem {
        id: item

        readonly property int indexBetween: 2 - Math.abs(root.hoveredIndex - index)
        scaleFactor: (root.hoveredIndex === -1 || indexBetween < 0) ? 1 : 1 + indexBetween * 0.2
        slideOn: root.slideOn
        revertAnimation: root.revertAnimation
        isRunningItem: root.runningActivity

        onHoveredChanged: {
            if (hovered) {
                root.hoveredIndex = index
            } else if (root.hoveredIndex === index) {
                root.hoveredIndex = -1
            }
        }

        onLeftMouseButtonClicked: {
            if (MagicPocket.activityManager.isRunning(activityName))
                MagicPocket.activityManager.activate(activityName)
            else
                MagicPocket.activityManager.start(activityName)
        }
    }
}
