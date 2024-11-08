import QtQuick

import CCMagicPocket

Item {
    id: root

    enum DisplayMode {
        FullScreen,
        Hidden,
        Layout
    }

    property int displayMode: ActivityContainer.DisplayMode.Hidden

    property alias showTransition: showHideTrans.showTransition
    property alias hideTransition: showHideTrans.hideTransition
    property alias closeTransition: closeTrans.closeTransition
    property var openTransition: Transition {
        ScaleAnimator {
            target: root
            from: 0.0
            to: 1.0
            duration: 600
            easing.type: Easing.OutCubic
        }
    }

    Item {
        id: container
        anchors.fill: parent
        children: [MagicPocket.activityManager.currentActivity]
    }

    ShowHideTransition {
        id: showHideTrans
        anchors.fill: container
        sourceItem: container
    }

    CloseTransition {
        id: closeTrans
        anchors.fill: container
        sourceItem: container
    }
}
