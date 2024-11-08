import QtQuick

Behavior {
    id: root

    property int duration: 600
    property Item fadeTarget: targetProperty.object

    NumberAnimation {
        target: fadeTarget
        properties: targetProperty.name
        duration: root.duration
        easing.type: Easing.OutBack
        easing.overshoot: 2.4
    }
}
