// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import "../Util/DbConvert.js" as DbConvert

Item {
    id: root

    enum Orientation {
        Left,
        Right
    }

    property var scaleData
    property int orientation: DbScale.Orientation.Left
    property bool withText: true
    property color backgroundColor: Qt.rgba(0, 0, 0, 1)
    property real padding: 0

    implicitWidth: withText ? 32 : 12

    Canvas {
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = backgroundColor;
            ctx.fillRect(0, 0, width, height);

            ctx.lineWidth = 1
            ctx.strokeStyle = Qt.rgba(1, 1, 1, 1)

            drawLines(ctx)

            if (root.withText) {
                ctx.font = "10px sans-serif"
                ctx.textAlign = "right"
                drawText(ctx)
            }
        }

        function tickPos(pos) {
            return (1 - pos) * (height - 2 * padding) + padding
        }

        function drawLines(ctx) {
            var lineX = orientation === DbScale.Orientation.Left ? 2 : root.width - 2
            var lineXE = orientation === DbScale.Orientation.Left ? 10 : root.width - 10

            ctx.beginPath()

            ctx.moveTo(lineX, 0)
            ctx.lineTo(lineX, height)

            var ticks = scaleData.ticks
            for (var i = 0; i < ticks.length; ++i) {
                var t = ticks[i]
                var yt = tickPos(t.position)

                drawTick(ctx, lineX, lineXE, yt)
            }

            ctx.moveTo(0, 0)
            ctx.closePath()
            ctx.stroke()
        }

        function drawTick(ctx, xs, xe, yt) {
            ctx.moveTo(xs, yt)
            ctx.lineTo(xe, yt)
        }

        function drawText(ctx) {
            var textX = root.orientation === DbScale.Orientation.Left ? root.width - 2 : root.width - 12

            var ticks = scaleData.ticks
            for (var i = 0; i < ticks.length; ++i) {
                var t = ticks[i]
                var yt = tickPos(t.position)

                drawTickText(ctx, textX, yt, t.dB)
            }
        }

        function drawTickText(ctx, textX, yt, db) {
            ctx.strokeText(DbConvert.dbToString(db), textX, yt + 2)
        }

        function dbToPosition(db) {
            return tickPos(root.scaleData.dbToPosition(db))
        }
    }
}
