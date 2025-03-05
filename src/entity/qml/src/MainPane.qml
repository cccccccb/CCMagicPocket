import QtQuick
import QtQuick.Effects

import CCMagicPocket

Item {
    z: -100
    property url backgroundImagePath

    Item {
        id: realBack
        anchors.fill: parent

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
    }
}
