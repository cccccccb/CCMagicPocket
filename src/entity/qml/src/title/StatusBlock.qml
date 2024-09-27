import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property alias expandingItems: expandingModel.children
    property int defaultBackgroundWidth: 100
    property int defaultBackgroundHeight: 6

    property bool _expanding: false
    property bool _expandingFinished: true

    hoverEnabled: true
    padding: 10

    background: Item {
        opacity: 0.9
        property int _backWidth: root._expanding ? _expandingLoader.width : root.defaultBackgroundWidth
        property int _backHeight: root._expanding && (expandingModel.count > 0) ? (_expandingLoader.height + 10) : root.defaultBackgroundHeight

        implicitWidth: _backWidth
        implicitHeight: root._expanding && (expandingModel.count > 0) ? (_expandingLoader.height + 20) : root.defaultBackgroundHeight * 4

        Rectangle {
            id: backItem
            width: parent._backWidth
            height: parent._backHeight
            anchors.centerIn: parent

            visible: false
            radius: root._expanding && (expandingModel.count > 0) ? 8 : 2
            color: Style.item.hightTextColor

            Behavior on radius {
                NumberAnimation {
                    duration: 300
                    easing.type: Easing.OutQuart
                }
            }
        }

        MultiEffect {
            id: sourceEffect
            source: backItem
            anchors.fill: backItem

            shadowEnabled: true
            shadowColor: Qt.color("#F8FFFFFF")
            shadowBlur: 0.4
            shadowOpacity: 0.6
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root._expandingFinished = false
                root._expanding = true
            }
        }

        Behavior on _backHeight {
                NumberAnimation {
                    duration:  300
                    easing.type: Easing.OutQuart
                }
        }

        Behavior on _backWidth {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutQuart
            }
        }

        Behavior on implicitHeight {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutQuart
            }
        }
    }

    contentItem: Item {
        id: statusContent
        implicitWidth: root._expanding ? _expandingLoader.width : 0
        implicitHeight: root._expanding ? _expandingLoader.height : 0

        Loader {
            id: _expandingLoader
            active: root._expanding || !root._expandingFinished
            anchors.centerIn: parent

            Binding {
                when: _expandingLoader.item !== null
                _expandingLoader.width: _expandingLoader.item.width
            }

            Binding {
                when: _expandingLoader.item !== null
                _expandingLoader.height: _expandingLoader.item.height
            }

            ObjectModel {
                id: expandingModel
            }

            sourceComponent: Item {
                width: childrenRect.width + 20
                height: childrenRect.height

                Row {
                    spacing: 10
                    width: childrenRect.width
                    height: childrenRect.height
                    x: 10

                    Repeater {
                        model: expandingModel
                    }
                }
            }
        }
    }

    state: "NOHOVERED_NOEXPANDING"
    states: [
        State {
            name: "HOVERED"
            when: hovered && !_expanding

            PropertyChanges {
                target: root
                scale: 1.4
            }

            PropertyChanges {
                target: root.background
                opacity: 0.9
            }

            PropertyChanges {
                target: statusContent
                opacity: 0
            }
        },
        State {
            name: "EXPANDING"
            when: _expanding

            PropertyChanges {
                target: root
                scale: 1
            }

            PropertyChanges {
                target: root.background
                opacity: 0.9
            }

            PropertyChanges {
                target: statusContent
                opacity: 1
            }
        },
        State {
            name: "NOHOVERED_NOEXPANDING"
            when: !_expanding && !hovered

            PropertyChanges {
                target: statusContent
                opacity: 0
            }

            PropertyChanges {
                target: root.background
                opacity: 0.6
            }

            PropertyChanges {
                target: root
                scale: 1
            }
        }
    ]

    transitions: [
        Transition {
            from: "HOVERED"
            to: "EXPANDING"

            ParallelAnimation {
                PropertyAction {
                    target: root
                    property: "_expandingFinished"
                    value: true
                }

                NumberAnimation {
                    target: statusContent
                    property: "opacity"
                    duration: 1400
                    easing.type: Easing.OutBack
                }

                ScaleAnimator {
                    target: root
                    duration: 400
                    easing.type: Easing.OutBack
                }
            }
        },
        Transition {
            from: "EXPANDING"
            to: "NOHOVERED_NOEXPANDING"

            SequentialAnimation {
                NumberAnimation {
                    target: statusContent
                    property: "opacity"
                    duration: 400
                    easing.type: Easing.OutBack
                }

                PropertyAction {
                    target: root
                    property: "_expandingFinished"
                    value: true
                }
            }

        },
        Transition {
            from: "*"
            to: "*"

            ParallelAnimation {
                PropertyAction {
                    target: root
                    property: "_expandingFinished"
                    value: true
                }

                NumberAnimation {
                    target: root.background
                    properties: "opacity"
                    duration: 400
                    easing.type: Easing.OutBack
                }

                ScaleAnimator {
                    target: root
                    duration: 400
                    easing.type: Easing.OutBack
                }
            }
        }
    ]

    Timer {
        id: expandingStateTimer
        interval: 1000
        repeat: false

        onTriggered: {
            root._expandingFinished = false
            root._expanding = false
        }
    }

    onHoveredChanged: {
        if (!hovered && _expanding) {
            expandingStateTimer.restart()
        } else if (hovered) {
            expandingStateTimer.stop()
        }
    }
}
