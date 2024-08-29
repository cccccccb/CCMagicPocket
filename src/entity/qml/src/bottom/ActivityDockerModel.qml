import QtQuick
import QtQml.Models

ListModel {
    id: model

    ListElement {
        icon: "qrc:/svg/icon-circle.svg"
        name: "Magic Pocket"
    }

    ListElement {
        icon: "qrc:/svg/firefox.svg"
        name: "FireFox"
    }

    ListElement {
        icon: "qrc:/svg/github.svg"
        name: "GitHub"
    }

    ListElement {
        icon: "qrc:/svg/gmail.svg"
        name: "GMail"
    }
}
