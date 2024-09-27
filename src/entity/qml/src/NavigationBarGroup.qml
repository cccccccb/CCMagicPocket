import QtQuick

import CCMagicPocket

import "left"
import "title"
import "bottom"

Rectangle {
    id: navigationRoot
    radius: 8

    property Item blurBackground

    clip: true
    color: "transparent"

    StatusTitleBar {
        id: statusBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 60
        slideOn: root.populate && !leftControlBar.visible && !MagicPocket.activityManager.runningContainer.visible
        blurBackground: navigationRoot.blurBackground
    }

    ActivityDocker {
        id: activityDocker

        anchors.horizontalCenter: parent.horizontalCenter
        slideOn: root.populate && !leftControlBar.visible && !MagicPocket.activityManager.runningContainer.visible
        blurBackground: navigationRoot.blurBackground
    }

    NavigationPopup {
        id: leftControlBar
        y: 20
        height: parent.height - 40

        visible: populate
        blurBackground: navigationRoot.blurBackground
    }

    SlidePopupButton {
        id: slideButton
        slideOn: (populate && !leftControlBar.visible) && !MagicPocket.activityManager.runningContainer.visible
        blurBackground: navigationRoot.blurBackground

        onClicked: {
            leftControlBar.visible = true
        }
    }
}
