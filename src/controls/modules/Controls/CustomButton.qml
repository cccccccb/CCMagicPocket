import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import CCMagicPocket

Button {
    id: root
    property int iconTextSpacing: Style.item.customButton.iconTextSpacing

    contentItem: Item {
        implicitHeight: textBesideIconLoader.active ? textBesideIconLoader.height
                                                    : textUnderIconLoader.active ? textUnderIconLoader.height
                                                                                 : iconOnlyLoader.active ? iconOnlyLoader.height
                                                                                                         : textOnlyLoader.height

        Component {
            id: iconComponent

            MPIcon {
                iconName: icon.name
                iconWidth: icon.width
                iconHeight: icon.height
                iconColor: root.icon.color
            }
        }

        Component {
            id: textComponent

            Text {
                text: root.text
                font: root.font
                color: root.icon.color
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Loader {
            id: textBesideIconLoader
            active: root.display === Button.TextBesideIcon
            anchors.centerIn: parent

            sourceComponent: Component {
                RowLayout {
                    spacing: root.iconTextSpacing

                    Loader {
                        Layout.alignment: Qt.AlignVCenter
                        sourceComponent: iconComponent
                    }

                    Loader {
                        Layout.alignment: Qt.AlignVCenter
                        sourceComponent: textComponent
                    }
                }
            }
        }

        Loader {
            id: textUnderIconLoader
            active: root.display === Button.TextUnderIcon
            anchors.centerIn: parent

            sourceComponent: Component {
                ColumnLayout {
                    spacing: root.iconTextSpacing

                    Loader {
                        Layout.alignment: Qt.AlignHCenter
                        sourceComponent: iconComponent
                    }

                    Loader {
                        Layout.alignment: Qt.AlignHCenter
                        sourceComponent: textComponent
                    }
                }
            }
        }

        Loader {
            id: iconOnlyLoader
            active: root.display === Button.IconOnly
            anchors.centerIn: parent

            sourceComponent: iconComponent
        }

        Loader {
            id: textOnlyLoader
            active: root.display === Button.TextOnly
            anchors.centerIn: parent

            sourceComponent: textComponent
        }
    }

    background: Rectangle {
        visible: !root.flat || root.down || root.checked || root.highlighted
        radius: root.Material.roundedScale === Material.FullScale ? height / 2 : root.Material.roundedScale
        color: root.Material.buttonColor(root.Material.theme, root.Material.background,
            root.Material.accent, root.enabled, root.flat, root.highlighted, root.checked)

        border.color: root.palette.highlight
        border.width: root.visualFocus ? 2 : 0
    }
}
