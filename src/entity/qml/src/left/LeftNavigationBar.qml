import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import CCMagicPocket

ToolBar {
    id: root

    signal homeButtonCheckedChanged
    signal favoriteButtonCheckedChanged

    Material.elevation: 0
    Material.background: "transparent"

    Item {
        x: 0
        y: buttonGroup.checkedButton.y
        width: parent.width
        height: buttonGroup.checkedButton.height

        Rectangle {
            id: slideRect
            anchors.left: parent.left
            width: 4
            height: parent.height
            radius: 2
            color: Material.accent
        }

        Rectangle {
            anchors.left: slideRect.right
            anchors.leftMargin: 2
            width: parent.width - slideRect.width
            height: parent.height

            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "#E0C0C0C0" }
                GradientStop { position: 1.0; color: "transparent" }
             }
        }

        Behavior on y {
            SpringAnimation {
                spring: 5
                damping: 0.3
                mass: 1.0
                epsilon: 0.25
            }
        }
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: Style.item.marginLevel1
        anchors.rightMargin: Style.item.marginLevel1
        anchors.topMargin: 0
        spacing: Style.item.smallSpacing

        CustomButton {
            id: home_button

            icon.name: checked ? "home_checked" : "home"
            icon.color: checked ? Qt.lighter(Material.accentColor) : Style.item.hightTextColor
            text: qsTr("Home")
            checked: true
            checkable: true
            width: parent.width
            display: Button.TextUnderIcon
            font.bold: checked

            onCheckedChanged: root.homeButtonCheckedChanged()
            background: null
        }

        CustomButton {
            id: category_button

            checkable: true
            icon.name: checked ? "category_checked" : "category"
            icon.color: checked ? Qt.lighter(Material.accentColor) : Style.item.hightTextColor

            text: qsTr("Category")
            width: parent.width
            display: Button.TextUnderIcon
            font.bold: checked

            onCheckedChanged: root.favoriteButtonCheckedChanged()
            background: null
        }

        CustomButton {
            id: label_button

            checkable: true
            icon.name: checked ? "label_checked" : "label"
            icon.color: checked ? Qt.lighter(Material.accentColor) : Style.item.hightTextColor

            text: qsTr("Label")
            width: parent.width
            display: Button.TextUnderIcon
            font.bold: checked

            onCheckedChanged: root.favoriteButtonCheckedChanged()
            background: null
        }

        CustomButton {
            id: community_button

            checkable: true
            icon.name: checked ? "community_checked" : "community"
            icon.color: checked ? Qt.lighter(Material.accentColor) : Style.item.hightTextColor

            text: qsTr("Square")
            width: parent.width
            display: Button.TextUnderIcon
            font.bold: checked

            onCheckedChanged: root.favoriteButtonCheckedChanged()
            background: null
        }

        CustomButton {
            id: favorite_button

            checkable: true
            icon.name: checked ? "favorite_checked" : "favorite"
            icon.color: checked ? Qt.lighter(Material.accentColor) : Style.item.hightTextColor

            text: qsTr("Favorite")
            width: parent.width
            display: Button.TextUnderIcon
            font.bold: checked

            onCheckedChanged: root.favoriteButtonCheckedChanged()
            background: null
        }

        ButtonGroup {
            id: buttonGroup
            buttons: [home_button, category_button, label_button, community_button, favorite_button]
        }

    }
}

