import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCStartup

AppStartupItem {
    id: root

    property int mainMargin
    Binding {
        when: root.loaded
        root.mainMargin: AppStartupItem.mainPane.showShadow ? Style.item.marginLevel1 : 0
    }

    AppStartupComponent {
        id: mainComp

        Rectangle {
            id: mainPane
            anchors.fill: parent
            anchors.margins: root.mainMargin
            color: "darkblue"
        }
    }
}
