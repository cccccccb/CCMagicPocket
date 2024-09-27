import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Rectangle {
    color: "white"
    border.color: "gray"
    border.width: 1
    radius: 8

    Column {
        anchors.centerIn: parent
        spacing: 20
        populate: Transition {
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 300 * 2
                    from: to - 40
                    easing.type: Easing.OutBounce
                }

                NumberAnimation {
                    properties: "opacity"
                    duration: 300
                    from: 0
                    to: 1
                }
            }
        }

        Image {
            source: "../res/live.png"
            sourceSize: Qt.size(64, 64)
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            text: "逗音"
            color: Qt.color("#EEE04F5F")
            font.bold: true
            font.pointSize: 13
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            Text {
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Getting ready to enter...")
                color: Qt.color("#A0E04F5F")
            }

            BusyIndicator {
                anchors.verticalCenter: parent.verticalCenter
                Material.accent: Qt.color("#A0E04F5F")
                running: true
                width: 24
                height: 24
            }

        }
    }
}

