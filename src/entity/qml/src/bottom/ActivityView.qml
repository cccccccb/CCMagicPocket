import QtQuick

ListView {
    id: root
    property int hoveredIndex: -1
    property bool slideOn: false
    property bool revertAnimation: false

    interactive: false
    implicitWidth: contentItem.childrenRect.width
    implicitHeight: 64
    orientation: ListView.Horizontal
    model: ActivityDockerModel {}

    delegate: ActivityItem {
        anchors.verticalCenter: parent.verticalCenter
        readonly property int indexBetween: 2 - Math.abs(root.hoveredIndex - index)
        scaleFactor: (root.hoveredIndex === -1 || indexBetween < 0) ? 1 : 1 + indexBetween * 0.3
        slideOn: root.slideOn
        revertAnimation: root.revertAnimation

        onHoveredChanged: {
            if (hovered) {
                root.hoveredIndex = index
            } else if (root.hoveredIndex === index) {
                root.hoveredIndex = -1
            }
        }
    }
}
