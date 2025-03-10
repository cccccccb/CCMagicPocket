import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects

import CCStartup
import CCMagicPocket

AppPreloadItem {
    id: root

    loadingOverlay: PreloadOverlay {
        id: overlay

        Button {
            id: skipButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.rightMargin: 10

            Material.background: Qt.color("#CCE04F5F")
            Material.foreground: Qt.color("#FFFFFFFF")
            Material.roundedScale: Material.ExtraLargeScale
            Material.elevation: 4

            visible: preloadCountdown.running
            text: qsTr("Skip") + " " + preloadCountdown.current

            onClicked: {
                preloadCountdown.running = false
            }
        }
    }

    autoExitOverlay: false
    overlayExitWhen: !preloadCountdown.running
    transitionGroup:  FlickTransition {}
    initialProperties: InitialProperties {
        readonly property bool visible: true
        readonly property int width: 1000
        readonly property int height: 760
        readonly property string title: "Magic Pocket"
        readonly property color color: "transparent"
        readonly property int flags: Qt.FramelessWindowHint | Qt.Window
    }

    Countdown {
        id: preloadCountdown
        interval: 10
        running: root.visible
    }
}
