import QtQuick

Transition {
    property Item sourceContainer
    property Item source

    ParentAnimation {
        target: source
        newParent: sourceContainer

        NumberAnimation {
            target: source
            properties: "x,y"
            to: 0
            duration: 400
            easing.type: Easing.OutQuart
        }
    }
}
