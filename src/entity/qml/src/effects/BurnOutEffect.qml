import QtQuick

Item {
    property alias source: effect.source
    property alias time: effect.time

    ShaderEffect {
        id: effect
        property variant source
        property variant noiseSrc: noiseImage

        property real time
        property real maskTile: 0.3
        property color edgeColor: Qt.rgba(1.0, 0.1, 0.0, 0.5)

        anchors.fill: parent
        fragmentShader: "qrc:/CCMagicPocket/qml/src/shadereffects/burn.frag.qsb"
    }

    Image {
        id: noiseImage
        source: "qrc:/CCMagicPocket/res/shaders/noise.png"
        visible: false
    }
}


