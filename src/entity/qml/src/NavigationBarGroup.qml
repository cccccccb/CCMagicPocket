import QtQuick

import CCMagicPocket

import "left"
import "title"
import "bottom"

Rectangle {
    id: navigationRoot
    radius: 8

    property Item backgroundItem

    clip: true
    color: "transparent"

    StatusTitleBar {
        id: statusBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 60
        slideOn: root.populate && !leftControlBar.visible
        backgroundItem: navigationRoot.backgroundItem

        Component.onCompleted: {
            Window.window.Frameless.moveUnder.push(this)
        }
    }

    ActivityDocker {
        id: activityDocker

        anchors.horizontalCenter: parent.horizontalCenter
        slideOn: root.populate && !leftControlBar.visible
        backgroundItem: navigationRoot.backgroundItem
    }

    NavigationPopup {
        id: leftControlBar
        y: 20
        height: parent.height - 40

        visible: populate
        backgroundItem: navigationRoot.backgroundItem
    }

    SlidePopupButton {
        id: slideButton
        slideOn: (populate && !leftControlBar.visible) || slideHoverEnter
        backgroundItem: navigationRoot.backgroundItem

        property bool slideHoverEnter: false
        onClicked: {
            leftControlBar.visible = true
        }
    }
}
