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
            Window.window.Frameless.canWindowResize = true
        }
    }
}
