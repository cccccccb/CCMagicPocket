import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

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
            color: "mistyrose"
            radius: 8
        }
    }

    AppStartupComponent {
        depends: mainComp
        Text {
            anchors.centerIn: parent
            color: "white"
            text: "这是示例程序"
            font.bold: true
            font.pointSize: 16
        }
    }

    AppStartupComponent {
        depends: mainComp

        Item {
            anchors.fill: parent

            Rectangle {
                id: barWithBackground
                width: 120; height: 50; x: 100; y: 50; radius: 10
                clip: true
                visible: false
                color: "black"
                Rectangle {
                    id: bar
                    color: "orange"
                    width: 60; height: 50; radius: 10
                    NumberAnimation on x {
                        running: true
                        from: -(bar.width * 1.2 )
                        to: barWithBackground.width * 1.2
                        loops: Animation.Infinite
                        duration: 1000
                    }
                }
            }

            MultiEffect {
                source: barWithBackground
                anchors.fill: barWithBackground
                maskEnabled: true
                maskSource: roundedRectangleMask
            }

            Item {
                id: roundedRectangleMask
                width: 120; height: 50
                layer.enabled: true
                visible: false
                Rectangle {
                    color: "black"
                    width: 120; height: 50; radius: 10
                    antialiasing: true
                }
            }
        }
    }
}
