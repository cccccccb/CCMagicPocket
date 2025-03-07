import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import CCStartup

AppStartupItem {
    id: root

    AppStartupComponent {
        id: mainComp

        Rectangle {
            id: mainPane
            anchors.fill: parent
            color: "mistyrose"
            radius: 8
        }
    }

    AppStartupComponent {
        depends: mainComp

        Item {
            anchors.fill: parent

            // Text {
            //     id: textItem
            //     anchors.centerIn: parent
            //     color: Qt.alpha("midnightblue", 0.6)
            //     text: "这是示例程序"
            //     font.bold: true
            //     font.pointSize: 30
            //     visible: false
            //     layer.enabled: true
            //     font.letterSpacing: 10
            // }

            Image {
                id: effectItem
                source: "../leftbackground.jpg"
                anchors.centerIn: parent
                visible: false
            }

            Image {
                id: noiseItem
                source: "../shaders/noise.png"
                sourceSize: effectItem.sourceSize
                visible: false
            }

            ShaderEffect {
                anchors.fill: effectItem
                property variant source: effectItem
                property variant noiseSrc: noiseItem
                property real maskTile: 0.3
                property color edgeColor: Qt.rgba(1.0, 0.1, 0.0, 0.5)
                // property real amplitude: 0.04 * 0.1
                // property real frequency: 20
                property real time: 0.0
                NumberAnimation on time {
                    /*loops: Animation.Infinite;*/
                    running: false;
                    id: timeAnimation; from: 0; to: 1.0; duration: 1000;

                    // onStarted: {
                    //     startText.text = Qt.formatDateTime(new Date(), "hh:mm:ss.zzz")
                    // }


                    // onStopped: {
                    //     stopText.text = Qt.formatDateTime(new Date(), "hh:mm:ss.zzz")
                    // }
                }
                fragmentShader: "shaders/wobble.frag.qsb"
            }

            // Text {
            //     id: startText
            //     anchors.left: button.right
            // }

            // Text {
            //     id: stopText
            //     anchors.left: startText.right
            // }

            // Button {
            //     id: button
            //     text: "donghua"
            //     x: 200
            //     y: 100
            //     onClicked: {
            //         timeAnimation.start()
            //     }
            // }
        }
    }
}
