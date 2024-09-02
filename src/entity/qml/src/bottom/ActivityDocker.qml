import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import CCMagicPocket

Control {
    id: root
    property bool slideOn: false
    property alias blurBackground: blurBack.blurBackground
    property alias permanentModel: permanentView.model
    property alias activeModel: activeView.model

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + topPadding

    padding: 10
    background: Item {
        opacity: 0.8

        Rectangle {
            id: _blurItem
            anchors.fill: parent

            visible: false
            color: Qt.lighter("#A0A0A0")
            border.color: "#E0E0E0"
            border.width: 1
            radius: 12
        }

        MultiEffect {
            source: _blurItem
            anchors.fill: _blurItem

            shadowEnabled: true
            shadowColor: Qt.color("#80000000")
            shadowBlur: 0.8
            shadowVerticalOffset: 4
        }

        BlurBackground {
            id: blurBack
            anchors.fill: parent
            radius: 12
        }
    }

    contentItem: Row {
        spacing: 10

        ActivityView {
            id: permanentView
            slideOn: root.slideOn
        }

        Loader {
            active: activeView.model.count > 0
            anchors.verticalCenter: parent.verticalCenter
            width: 2
            height: Math.max(permanentView.height, activeView.height)

            sourceComponent: Rectangle {
                width: 2
                radius: 1
                color: Qt.alpha(Style.item.hightTextColor, 0.6)
            }
        }

        ActivityView {
            id: activeView
            slideOn: root.slideOn
            revertAnimation: true

            model: ListModel {
                ListElement {
                    icon: "qrc:/svg/google-chrome.svg"
                    name: "Google Chrome"
                }

                ListElement {
                    icon: "qrc:/svg/safari.svg"
                    name: "Safari"
                }

                ListElement {
                    icon: "qrc:/svg/tiktok.svg"
                    name: "TikTok"
                }

                ListElement {
                    icon: "qrc:/svg/twitter.svg"
                    name: "Twitter"
                }

                ListElement {
                    icon: "qrc:/svg/wechat.svg"
                    name: "Wechat"
                }
            }
        }
    }

    state: "NONSLIDEON"
    states: [
        State {
            name: "SLIDEON"
            when: slideOn
            PropertyChanges {
                target: root
                opacity: 1.0
                y: parent.height - height - 20
            }
        },
        State {
            name: "NONSLIDEON"
            when: !slideOn
            PropertyChanges {
                target: root
                opacity: 0.0
                y: parent.height
            }
        }
    ]

    transitions: [
        Transition {
            from: "NONSLIDEON"
            to: "SLIDEON"

            PropertyAnimation {
                target: root
                duration: 600
                properties: "opacity,y"
                easing.type: Easing.OutQuint
            }
        },
        Transition {
            from: "SLIDEON"
            to: "NONSLIDEON"

            PropertyAnimation {
                target: root
                duration: 600
                properties: "opacity,y"
                easing.type: Easing.InQuint
            }
        }
    ]
}
