import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Effects

import CCMagicPocket

TextField {
    id: control

    opacity: enabled ? 1 : 0.4
    leftPadding: (editting) ? searchIcon.width + 2 * Style.item.searchEdit.iconMargin : 0
    selectionColor: Material.accent
    selectedTextColor: control.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    background: Item {
        Loader {
            anchors.fill: parent
            active: !control.activeFocus

            sourceComponent: Rectangle {
                radius: Style.item.searchEdit.radius
                color: Style.item.searchEdit.backgroundColor
            }
        }

        Loader {
            anchors {
                fill: parent
                margins: -Style.item.focusPadding
            }

            active: control.activeFocus
            sourceComponent: Item {
                Rectangle {
                    id: _blurRect
                    anchors.fill: parent
                    color: Style.item.searchEdit.edittingBackgroundColor
                    radius: Style.item.searchEdit.radius
                    border { width: 1; color: Qt.rgba(0.8, 0.8, 0.8, 0.7) }

                    OuterShadow {
                        anchors.fill: _blurRect
                        shadowRadius: 20
                        shadowColor: Qt.rgba(0, 0, 0, 1)
                        cornerRadius: _blurRect.radius
                    }
                }
            }
        }
    }
}
