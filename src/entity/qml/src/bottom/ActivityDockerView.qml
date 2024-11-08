import QtQuick

import CCMagicPocket

ListView {
    id: root
    property int hoveredIndex: -1
    property bool slideOn: false
    property bool revertAnimation: false
    property bool runningActivity: false
    property var runningActivityItem

    interactive: false
    implicitWidth: childrenRect.width
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
                MagicPocket.activityManager.open(activityName)
        }

        Connections {
            enabled: item.isRunningItem
            target: item

            function onIsCurrentItemChanged() {
                if (isCurrentItem) {
                    item.ListView.view.runningActivityItem = item
                }
            }
        }
    }

    add: Transition {
        NumberAnimation {
            properties: "opacity"
            from: 0.0
            to: 1.0
            duration: 500
        }
    }

    Connections {
        enabled: root.runningActivity
        target: MagicPocket.activityManager

        function onCurrentActivityChanged() {
            if (MagicPocket.activityManager.currentActivity === null) {
                root.runningActivityItem = null
            }
        }
    }
}
