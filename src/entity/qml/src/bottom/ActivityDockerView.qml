import QtQuick

import CCMagicPocket

ListView {
    id: root
    property int hoveredIndex: -1
    property bool slideOn: false
    property bool revertAnimation: false
    property bool runningActivity: false
    property var runningActivityItem

    property int _hoveredDelayIndex: -1

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
                root._hoveredDelayIndex = index
            } else if (root._hoveredDelayIndex === index) {
                root._hoveredDelayIndex = -1
            }

            if (delayUpdateIndexTimer.running)
                return

            delayUpdateIndexTimer.start()
        }

        onLeftMouseButtonClicked: {
            if (MagicPocket.activityManager.isRunning(activityName))
                MagicPocket.activityManager.activate(activityName)
            else
                MagicPocket.activityManager.open(activityName)
        }

        Connections {
            enabled: item.isRunningItem
            target: MagicPocket.activityManager

            function onCurrentActivityNameChanged() {
                if (MagicPocket.activityManager.currentActivityName === activityName && activityName !== "") {
                    item.ListView.view.runningActivityItem = item
                }
            }
        }

        Component.onCompleted: {
            if (item.isRunningItem && MagicPocket.activityManager.currentActivityName === activityName && activityName !== "") {
                item.ListView.view.runningActivityItem = item
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

    Timer {
        id: delayUpdateIndexTimer

        interval: 0
        onTriggered: {
            root.hoveredIndex = root._hoveredDelayIndex
        }
    }
}
