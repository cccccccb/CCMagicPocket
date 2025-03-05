import QtQuick

Item {
    property alias source:effect.source
    property int radius: 0

    ShaderEffect {
        id: effect

        anchors.fill: parent
        property variant source: sourceEffect
        property real adjustX
        property real adjustY

        fragmentShader: "qrc:/CCMagicPocket/qml/src/shadereffects/invertcoloreffect.frag.qsb"

        Component.onCompleted: {
            adjustX = Qt.binding(function () {
                var ret = parent.radius * 1.0 / width;
                return ret > 0.5 ? 0.5 : ret;
            })

            adjustY = Qt.binding(function () {
                var ret = parent.radius * 1.0 / height;
                return ret > 0.5 ? 0.5 : ret;
            })
        }
    }
}
