import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket
import CCStartup

import "left"
import "title"
import "bottom"


AppStartupItem {
    id: root
    asynchronous: true

    AppStartupComponent {
        id: mainPaneComp

        MainPane {
            id: mainPane
            z: -100
            anchors.fill: parent
            backgroundImagePath: Qt.url("../../res/default_wallpaper_2.jpg")
        }
    }

    AppStartupComponent {
        id: centerPaneComponent
        depends: mainPaneComp

        CenterPane {
            parent: AppStartupItem.mainPane
            anchors.fill: parent

            // Rectangle {
            //     width: 600
            //     height: 600
            //     anchors.centerIn: parent

            //     Rectangle {
            //         width: 200
            //         height: 200
            //         color: "white"
            //         radius: 8
            //         anchors.centerIn: parent

            //         OuterShadow {
            //             anchors.fill: parent
            //             shadowColor: Qt.rgba(0, 0, 0, 0.5)
            //             cornerRadius: 8
            //             shadowRadius: 20
            //         }
            //     }
            // }
        }
    }


    AppStartupComponent {
        depends: centerPaneComponent

        NavigationBarGroup {
            anchors.fill: parent
            blurBackground: AppStartupItem.mainPane
        }
    }

    onPopulateChanged: {
        if (populate) {
            Window.window.Frameless.enabled = true
            Window.window.Frameless.canWindowResize = true
        }
    }
}
