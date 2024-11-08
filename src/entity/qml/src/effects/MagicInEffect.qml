import QtQuick

ShaderEffect {
    property variant source
    property real bend: 1.0
    property real minimize: 1.0
    property real side: 0.8
    mesh: Qt.size(20, 20)

    vertexShader: "qrc:/CCMagicPocket/qml/src/shadereffects/genie.vert.qsb"
}
