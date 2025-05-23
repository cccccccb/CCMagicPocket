import QtQuick

Item {
    property alias source: effect.source
    property alias time: effect.time

    ShaderEffect {
        id: effect
        property variant source
        property variant noise: noiseImage

        property real time
        property real crackCount: 30

        anchors.fill: parent
        fragmentShader: "qrc:/CCMagicPocket/qml/src/shadereffects/glassbreak.frag.qsb"
    }

    Image {
        id: noiseImage
        source: "qrc:/CCMagicPocket/res/shaders/noise.png"
        visible: false
    }
}
