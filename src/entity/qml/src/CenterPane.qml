import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import CCMagicPocket


Item {
    id: root

    clip: true
    ListView {
        id: activityView
        property bool delayHide: false

        visible: false
        anchors.fill: parent
        orientation: Qt.Horizontal
        model: ObjectModel {
            children: MagicPocket.activityManager.runningActivity
        }

        state: "NO_VISIBLE"
        states: [
            State {
                name: "NO_VISIBLE"
                when: !activityView.visible && !activityView.delayHide
                PropertyChanges {
                    target: activityView
                    scale: 0
                }
            },

            State {
                name: "DELAY_HIDE"
                when: activityView.visible && activityView.delayHide
                PropertyChanges {
                    target: activityView
                    scale: 0
                }
            },

            State {
                name: "VISIBLE"
                when: activityView.visible && !activityView.delayHide
                PropertyChanges {
                    target: activityView
                    scale: 1.0
                }
            }
        ]

        transitions: [
            Transition {
                from: "NO_VISIBLE"
                to: "VISIBLE"

                ScaleAnimator {
                    duration: 300
                    easing.type: Easing.OutQuad
                }
            },

            Transition {
                from: "VISIBLE"
                to: "DELAY_HIDE"

                SequentialAnimation {
                    PropertyAction { target: activityView; property: "delayHide"; value: true }
                    ScaleAnimator { target: activityView; duration: 300; easing.type: Easing.InQuad }
                    PropertyAction { target: activityView; property: "delayHide"; value: false }
                 }
            }
        ]
    }

    Component {
        id: runningTemplate

        ActivityTemplate {}
    }

    Component.onCompleted: {
        MagicPocket.activityManager.runningContainer = activityView
        MagicPocket.activityManager.runningTemplate = runningTemplate
    }
}
