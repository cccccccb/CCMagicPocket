import QtQml
import QtQuick
import QtQuick.Controls

import CCMagicPocket

Control {
    id: root

    hoverEnabled: true
    background: Item {
        Rectangle {
            id: backItem
            width: parent.width
            height: 5
            anchors.centerIn: parent

            radius: 2
            color: Qt.alpha(Style.item.hightTextColor, 0.6)
        }
    }

    state: "NONHOVERED"
    states: [
        State {
            name: "HOVERED"
            when: hovered

            PropertyChanges {
                target: backItem
                color: Qt.alpha(Style.item.hightTextColor, 0.8)
                scale: 1.4
            }
        },
        State {
            name: "NONHOVERED"
            when: !hovered

            PropertyChanges {
                target: backItem
                color: Qt.alpha(Style.item.hightTextColor, 0.6)
                scale: 1
            }
        }
    ]

    transitions: Transition {
        ParallelAnimation {
            ColorAnimation {
                duration: 400
            }

            ScaleAnimator {
                duration: 300
                easing.type: Easing.OutBack
            }
        }
    }
}
