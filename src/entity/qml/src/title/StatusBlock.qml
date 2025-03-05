import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "../effects"

import CCMagicPocket

Control {
    id: root
    property alias expandingItems: expandingModel.children
    property int defaultBackgroundWidth: 100
    property int defaultBackgroundHeight: 6

    property bool _expanding: false
    property bool _expandingFinished: true

    property int _cornerRadius

    property Item blurBackground

    hoverEnabled: true
    padding: 10

    background: Item {
        property int _backWidth
        property int _backHeight

        implicitWidth: _backWidth

        Loader {
            active: root.blurBackground !== null
            width: parent._backWidth
            height: parent._backHeight
            anchors.centerIn: parent
            opacity: 0.8

            sourceComponent: Item {
                anchors.fill: parent

                ShaderEffectSource {
                    id: sourceEffect

                    function mapToTarget(source, target, rect)
                    {
                        return source.mapToItem(target, source.mapFromItem(source.parent, rect))
                    }

                    visible: false
                    width: parent.width
                    height: parent.height
                    sourceItem: root.blurBackground
                    sourceRect: mapToTarget(root, root.blurBackground, Qt.rect(root.x, root.y, root.width, root.height))
                    recursive: false
                    live: false
                }

                InvertColorEffect {
                    anchors.fill: parent
                    source: sourceEffect
                    radius: root._cornerRadius
                }
            }
        }

        Loader {
            active: root.blurBackground === null
            width: parent._backWidth
            height: parent._backHeight
            anchors.centerIn: parent

            sourceComponent: Rectangle {
                radius: root._cornerRadius
                color: Qt.alpha(Style.item.hightTextColor, root.blurBackground !== null ? 0.4 : 1.0)
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                root._expandingFinished = false
                root._expanding = true
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
                _expandingLoader.width: _expandingLoader.item.childrenWidth
            }

            Binding {
                when: _expandingLoader.item !== null
                _expandingLoader.height: _expandingLoader.item.childrenHeight
            }

            ObjectModel {
                id: expandingModel
            }

            sourceComponent: Item {
                enabled: root._expandingFinished
                property int childrenWidth: childrenRect.width + 20
                property int childrenHeight: childrenRect.height

                Row {
                    x: 10
                    spacing: 10
                    width: childrenRect.width
                    height: childrenRect.height

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
                target: root.background
                _backWidth: root.defaultBackgroundWidth
                _backHeight: root.defaultBackgroundHeight
                implicitHeight: root.defaultBackgroundHeight * 4
            }

            PropertyChanges {
                target: root
                scale: 1.4
                _cornerRadius: 2
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
                target: root.background
                _backWidth: _expandingLoader.width
                _backHeight: (expandingModel.count > 0) ? (_expandingLoader.height + 10) : root.defaultBackgroundHeight
                implicitHeight: (expandingModel.count > 0) ? (_expandingLoader.height + 20) : root.defaultBackgroundHeight * 4
            }

            PropertyChanges {
                target: root
                scale: 1
                _cornerRadius: (expandingModel.count > 0) ? 8 : 2
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
                target: root.background
                _backWidth: root.defaultBackgroundWidth
                _backHeight: root.defaultBackgroundHeight
                implicitHeight: root.defaultBackgroundHeight * 4
            }

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
                _cornerRadius: 2
            }
        }
    ]

    transitions: [
        Transition {
            from: "HOVERED"
            to: "EXPANDING"

            SequentialAnimation {
                ParallelAnimation {
                    NumberAnimation {
                        target: statusContent
                        property: "opacity"
                        duration: 400
                        easing.type: Easing.OutBack
                    }

                    ScaleAnimator {
                        target: root
                        duration: 400
                        easing.type: Easing.OutBack
                    }

                    NumberAnimation {
                        target: root
                        property: "_cornerRadius"
                        duration: 300
                    }

                    NumberAnimation {
                        target: root.background
                        properties: "_backWidth,_backHeight,implicitHeight"
                        duration: 400
                        easing.type: Easing.OutQuart
                    }
                }

                PropertyAction {
                    target: root
                    property: "_expandingFinished"
                    value: true
                }
            }
        },
        Transition {
            from: "EXPANDING"
            to: "NOHOVERED_NOEXPANDING"

            ParallelAnimation {
                NumberAnimation {
                    target: statusContent
                    property: "opacity"
                    duration: 400
                    easing.type: Easing.OutBack
                }

                NumberAnimation {
                    target: root.background
                    properties: "_backWidth,_backHeight,implicitHeight"
                    duration: 400
                    easing.type: Easing.OutQuart
                }

                NumberAnimation {
                    target: root
                    property: "_cornerRadius"
                    duration: 300
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

                NumberAnimation {
                    target: root.background
                    properties: "_backWidth,_backHeight,implicitHeight"
                    duration: 400
                    easing.type: Easing.OutQuart
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
