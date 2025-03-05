import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket
import CCStartup

import "../controls"

Control {
    id: root

    padding: 6
    implicitWidth: contentItem.implicitWidth + 2 * padding

    contentItem: Item {
        id: contentRoot
        readonly property int controlSpacing: 6
        implicitWidth: closeButton.width + maxButton.width + minButton.width + fullscreenButton.width + controlSpacing * 3

        RoundAnimateButton {
            id: minButton

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: "minimize"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.4, 0.7, 0.4, 0.6)
            backgroundBorderColor: Qt.hsla(0.4, 1, 0.2, 0.8)

            onClicked: {
                Window.window.showMinimized()
            }
        }

        RoundAnimateButton {
            id: maxButton

            readonly property var rootItem: maxButton.AppStartupItem.startupItem
            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: minButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: (rootItem.Window.window.visibility === Window.Windowed
                            || rootItem.Window.window.visibility === Window.Minimized)
                           ? "maximize" : "normalize"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.2, 0.7, 0.4, 0.8)
            backgroundBorderColor: Qt.hsla(0.2, 1, 0.2, 0.8)

            onClicked: {
                if (Window.window.visibility === Window.Maximized) {
                    Window.window.showNormal()
                } else {
                    Window.window.showMaximized()
                }
            }
        }

        RoundAnimateButton {
            id: fullscreenButton

            readonly property var rootItem: fullscreenButton.AppStartupItem.startupItem
            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: maxButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: rootItem.Window.window.visibility === Window.FullScreen ? "fullscreen_exit"
                                                                                   : "fullscreen"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.6, 0.7, 0.4, 0.6)
            backgroundBorderColor: Qt.hsla(0.6, 1, 0.2, 0.8)

            onClicked: {
                if (Window.window.visibility !== Window.FullScreen) {
                    Window.window.showFullScreen()
                } else {
                    Window.window.showNormal()
                }
            }
        }

        RoundAnimateButton {
            id: closeButton

            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: fullscreenButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: "close"
            icon.iconColor: "white"
            backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
            backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)

            onClicked: {
                Window.window.close()
            }
        }
    }
}
