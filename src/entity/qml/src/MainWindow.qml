import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket
import CCMagicPocket.impl

import CCStartup

import "left"
import "title"
import "bottom"


AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainPlaneComp

        MainDesktopPlane {
            id: mainPlane
            anchors.fill: parent
            backgroundImagePath: Qt.url("../../res/default_wallpaper_2.jpg")
        }
    }

    AppStartupComponent {
        id: layoutPlaneComponent
        depends: mainPlaneComp

        LayoutPlane {
            anchors.fill: parent
            parent: AppStartupItem.mainPlane

            Component.onCompleted: {
                AppStartupItem.mainPlane.layoutPlane = this
            }
        }
    }

    AppStartupComponent {
        depends: layoutPlaneComponent

        WindowPane {
            anchors.fill: parent

            Component.onCompleted: {
                AppStartupItem.mainPlane.windowPlane = this
                AppStartupItem.mainPlane.currentPlane = this
            }
        }
    }

    AppStartupComponent {
        depends: layoutPlaneComponent

        FullscreenPlane {
            anchors.fill: parent

            Component.onCompleted: {
                AppStartupItem.mainPlane.fullscreenPlane = this
            }
        }
    }

    onPopulateChanged: {
        if (populate) {
            Window.window.Frameless.enabled = true
            Window.window.Frameless.canWindowResize = true
        }
    }
}
