import QtQuick
import QtQuick.Effects

import CCMagicPocket
import CCMagicPocket.impl

import "activity"

ActivityPlaneGroup {
    id: root
    property url backgroundImagePath

    Loader {
        anchors.fill: parent
        active: backgroundImagePath.toString() === ""

        sourceComponent: Rectangle {
            color: Style.item.backgroundColor
            border.color: Style.item.borderColor
            border.width: Style.item.borderWidth
        }
    }

    Loader {
        clip: true
        anchors.fill: parent
        asynchronous: true
        active: backgroundImagePath.toString() !== ""

        sourceComponent: Image {
            id: sourceItem
            asynchronous: true
            source: backgroundImagePath
            fillMode: Image.PreserveAspectCrop
        }
    }

    ActivityManager {
        runningTemplate: Qt.createComponent("activity/ActivityTemplate.qml", root.fullscreenPlane)

        Component.onCompleted: {
            MagicPocket.activityManager = this
        }
    }

    Component.onCompleted: {
        MagicPocket.planeGroupManager.roots.push(this)
    }

    Connections {
        target: MagicPocket.activityManager

        function onOpened(activityName) {
            root.currentPlane = root.fullscreenPlane
            MagicPocket.activityManager.currentActivity.parent = root.fullscreenPlane
        }

        function onClosed(activityName) {
            if (root.currentPlane == root.fullscreenPlane)
                root.currentPlane = root.windowPlane
        }
    }

    Connections {
        enabled: MagicPocket.activityManager !== null && MagicPocket.activityManager.currentActivity !== null
        target: MagicPocket.activityManager.currentActivity
        ignoreUnknownSignals: true

        function onClosed(activityName) {
            MagicPocket.activityManager.close(activityName)
        }

        function onMinimized(activityName) {
            MagicPocket.activityManager.minimize(activityName)
        }

        function onMinizedTransitionFinished() {
            root.currentPlane = root.windowPlane
        }

        function onShowTransitionStarted() {
            root.currentPlane = root.fullscreenPlane
        }

        function onLayouted(activityName) {
            MagicPocket.activityManager.layout(activityName)
            root.currentPlane = root.layoutPlane
        }
    }
}
