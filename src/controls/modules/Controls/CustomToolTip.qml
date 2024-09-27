import QtQuick
import QtQuick.Shapes
import QtQuick.Effects
import QtQuick.Controls.Basic

import CCMagicPocket

ToolTip {
    id: control

    padding: 10
    bottomPadding: 10 + back._arrowArcY + back._arrowTop
    contentItem: Text {
        text: control.text
        font.pointSize: 8
        font.bold: true
        color: Style.item.backgroundTitleColor
        verticalAlignment: Text.AlignVCenter
    }

    background: Item {
        id: back

        readonly property int _rectRadius: 8
        readonly property int _arrowArcX: 8
        readonly property int _arrowArcY: 4
        readonly property int _arrowTop: 2

        Shape {
            id: shape
            anchors.fill: parent
            layer.enabled: true
            layer.samples: 4
            width: back.width
            height: back.height

            ShapePath {
                strokeColor: "transparent"
                fillColor: "#80000000"

                startX: back._rectRadius
                startY: 0

                PathArc {
                    x: 0
                    y: back._rectRadius
                    radiusX: back._rectRadius
                    radiusY: back._rectRadius
                    direction: PathArc.Counterclockwise
                }

                PathLine {
                    x: 0
                    y: shape.height - (back._rectRadius + back._arrowArcY + back._arrowTop)
                }

                PathArc {
                    x: back._rectRadius
                    y: shape.height - (back._arrowArcY + back._arrowTop)
                    radiusX: back._rectRadius
                    radiusY: back._rectRadius
                    direction: PathArc.Counterclockwise
                }

                PathLine {
                    x: shape.width / 2 - (back._arrowArcX + back._arrowTop)
                    y: shape.height - (back._arrowArcY + back._arrowTop)
                }

                PathArc {
                    x: shape.width / 2 - back._arrowTop
                    y: shape.height - back._arrowTop
                    radiusX: back._arrowArcX
                    radiusY: back._arrowArcY
                    direction: PathArc.Clockwise
                }

                PathArc {
                    x: shape.width / 2 + back._arrowTop
                    y: shape.height - back._arrowTop
                    radiusX: back._arrowTop
                    radiusY: back._arrowTop
                    direction: PathArc.Counterclockwise
                }

                PathArc {
                    x: shape.width / 2 + back._arrowArcX
                    y: shape.height - (back._arrowArcY + back._arrowTop)
                    radiusX: back._arrowArcX
                    radiusY: back._arrowArcY
                    direction: PathArc.Clockwise
                }

                PathLine {
                    x: shape.width - back._rectRadius
                    y: shape.height - (back._arrowArcY + back._arrowTop)
                }

                PathArc {
                    x: shape.width
                    y: shape.height - (back._rectRadius + back._arrowArcY + back._arrowTop)
                    radiusX: back._rectRadius
                    radiusY: back._rectRadius
                    direction: PathArc.Counterclockwise
                }

                PathLine {
                    x: shape.width
                    y: back._rectRadius
                }

                PathArc {
                    x: shape.width - back._rectRadius
                    y: 0
                    radiusX: back._rectRadius
                    radiusY: back._rectRadius
                    direction: PathArc.Counterclockwise
                }

                PathLine {
                    x: back._rectRadius
                    y: 0
                }
            }
        }
    }
}
