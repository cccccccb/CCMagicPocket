import QtQuick

import CCMagicPocket
import CCMagicPocket.impl

import "left"
import "title"
import "bottom"

ActivityPlaneItem {
    id: navigationRoot
    property bool _slideOn: true

    StatusTitleBar {
        id: statusBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 60
        slideOn: populate /*&& !leftControlBar.visible*/ && navigationRoot._slideOn
        blurBackground: navigationRoot.group
    }

    ActivityDocker {
        id: activityDocker

        anchors.horizontalCenter: parent.horizontalCenter
        slideOn: statusBar.slideOn
        blurBackground: navigationRoot.group
    }

    pushUp: Transition {
        PropertyAction {
            target: navigationRoot
            properties: "_slideOn"
            value: true
        }
    }

    popUp: Transition {
        SequentialAnimation {
            PropertyAction {
                target: navigationRoot
                properties: "_slideOn"
                value: false
            }

            PauseAnimation {
                duration: 400
            }
        }
    }

    // NavigationPopup {
    //     id: leftControlBar
    //     y: 20
    //     height: parent.height - 40

    //     visible: populate
    //     blurBackground: navigationRoot.blurBackground
    // }

    // SlidePopupButton {
    //     id: slideButton
    //     slideOn: (populate && !leftControlBar.visible) && !MagicPocket.activityManager.runningContainer.visible && !MagicPocket.activityManager.runningLayoutView.visible
    //     blurBackground: navigationRoot.blurBackground

    //     onClicked: {
    //         leftControlBar.visible = true
    //     }
    // }
}
