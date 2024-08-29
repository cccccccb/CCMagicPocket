import QtQuick
import QtQml

import CCMagicPocket

Rectangle {
    id: root

    property bool activateOn: false

    opacity: 0
    height: 4
    radius: 2
    color: Style.item.hightTextColor

    Behavior on color {
        enabled: !activatyTimer.running
        ColorAnimation {
            duration: 1000
        }
    }

    Timer {
        id: activatyTimer
        interval: 500
        repeat: false

        onTriggered: {
            if (activateOn) {
                root.color = Qt.binding(function() {
                    return Qt.alpha(Style.item.hightTextColor, 0.6)
                })
            } else {
                root.color = Qt.binding(function() {
                    return Style.item.hightTextColor
                })
            }
        }
    }

    onActivateOnChanged: {
        activatyTimer.restart()
    }
}
