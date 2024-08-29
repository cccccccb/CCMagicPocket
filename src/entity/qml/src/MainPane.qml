import QtQuick
import QtQuick.Effects

import CCMagicPocket

Item {
    z: -100
    readonly property bool showShadow: !(Window.window.visibility === Window.Maximized || Window.window.visibility === Window.FullScreen)
    property url backgroundImagePath

    Item {
        id: realBack
        anchors.fill: parent
        visible: !showShadow

        Loader {
            anchors.fill: parent
            active: backgroundImagePath.toString() === ""

            sourceComponent: Rectangle {
                color: Style.item.backgroundColor
                border.color: Style.item.borderColor
                border.width: Style.item.borderWidth
                radius: showShadow ? Style.item.maxRadius : 0
            }
        }

        Loader {
            clip: true
            anchors.fill: parent
            active: backgroundImagePath.toString() !== ""

            sourceComponent: ClipAreaItem {
                radius: showShadow ? Style.item.maxRadius : 0
                component : Image {
                    id: sourceItem
                    source: backgroundImagePath
                    fillMode: Image.PreserveAspectCrop
                }
            }
        }
    }

    Loader {
        active: showShadow
        anchors.fill: realBack

        sourceComponent: MultiEffect {
            source: realBack
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowColor: Qt.color("#CF000000")
            shadowBlur: 0.5
        }
    }

    Loader {
        active: showShadow
        anchors.fill: realBack

        sourceComponent: InnerShadow {
            component: Rectangle {
                color: "transparent"
                border.color: "#80A0A0A0"
                border.width: Style.item.borderWidth
                radius: showShadow ? Style.item.maxRadius : 0
            }

            radius: showShadow ? Style.item.maxRadius : 0
        }
    }
}
