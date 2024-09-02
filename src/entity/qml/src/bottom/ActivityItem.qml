import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property real scaleFactor: 1.0
    property bool slideOn: false
    property bool revertAnimation: false

    hoverEnabled: true
    padding: 10
    background: Rectangle {
        anchors.fill: parent
        anchors.margins: 4

        color: "white"
        radius: 12

        transform: root._contentTranslate
    }

    contentItem: Item {
        anchors.centerIn: parent
        implicitWidth: 48 * scaleFactor
        implicitHeight: 48 * scaleFactor

        Image {
            id: _blurItem
            anchors.fill: parent
            visible: false

            source: icon
            fillMode: Image.PreserveAspectFit
            sourceSize: Qt.size(48 * scaleFactor, 48 * scaleFactor)
        }

        MultiEffect {
            source: _blurItem
            anchors.fill: _blurItem

            shadowEnabled: true
            shadowColor: Qt.color("#A0000000")
            shadowBlur: 0.8
            shadowOpacity: 0.4
            shadowVerticalOffset: 4
        }

        transform: root._contentTranslate
    }

    Behavior on scaleFactor {
        NumberAnimation {
            duration: 150
        }
    }

    readonly property var _contentTranslate: Translate {
        y: -(height - 48 - root.topPadding - root.bottomPadding)
    }

    property int _slideOnY: 0

    state: "NONSLIDE_ON"
    states: [
        State {
            name: "SLIDE_ON"
            when: slideOn
            PropertyChanges {
                target: root
                _slideOnY: 0
            }
        },
        State {
            name: "NONSLIDE_ON"
            when: !slideOn
            PropertyChanges {
                target: root
                _slideOnY: root.height * 2
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDE_ON"
            to: "SLIDE_ON"

            SequentialAnimation {
                PauseAnimation {
                    duration: revertAnimation ? (root.ListView.view.count - 1 - index) * 60 : index * 60
                }

                PropertyAnimation {
                    duration: 800
                    properties: "_slideOnY"
                    easing.type: Easing.OutQuint
                }
            }
        },

        Transition {
            from: "SLIDE_ON"
            to: "NONSLIDE_ON"

            SequentialAnimation {
                PauseAnimation {
                    duration: revertAnimation ? (root.ListView.view.count - 1 - index) * 60 : index * 60
                }

                PropertyAnimation {
                    duration: 400
                    properties: "_slideOnY"
                    easing.type: Easing.InQuint
                }
            }
        }
    ]

    transform: Translate {
        y: _slideOnY
    }
}
