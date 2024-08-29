import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket
import CCStartup

import "left"
import "title"


AppStartupItem {
    id: root

    Component {
        MainPane {
            id: mainPane
            z: -100
            anchors.fill: parent
            anchors.margins: showShadow ? Style.item.marginLevel1 : 0
            backgroundImagePath: Qt.url("qrc:/default_wallpaper_2.jpg")

            CenterPane {
                id: centerPane
                anchors.fill: parent
            }
        }
    }

    Component {
        NavigationBarGroup {
            anchors.fill: parent
            anchors.margins: AppStartupItem.mainPane.anchors.margins
            backgroundItem: AppStartupItem.mainPane
        }
    }

    onPopulateChanged: {
        if (populate) {
            Window.window.Frameless.canWindowResize = true
            Window.window.Frameless.contentMargins = AppStartupItem.mainPane.anchors.margins
        }
    }
}
