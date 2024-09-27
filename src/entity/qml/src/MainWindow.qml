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
            anchors.margins: showShadow ? Style.item.marginLevel1 : 0
            backgroundImagePath: Qt.url("../../res/default_wallpaper_2.jpg")

            CenterPane {
                id: centerPane
                anchors.fill: parent
            }
        }
    }

    AppStartupComponent {
        depends: mainPaneComp

        NavigationBarGroup {
            anchors.fill: parent
            anchors.margins: AppStartupItem.mainPane.anchors.margins
            blurBackground: AppStartupItem.mainPane
        }
    }

    onPopulateChanged: {
        if (populate) {
            Window.window.Frameless.canWindowResize = true
            Window.window.Frameless.contentMargins = AppStartupItem.mainPane.anchors.margins
        }
    }
}
