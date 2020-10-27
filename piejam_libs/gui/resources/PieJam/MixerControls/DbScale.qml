// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.13

import "DbConvert.js" as DbConvert

Canvas {
    id: root

    enum Orientation {
        Left,
        Right
    }

    property DbScaleData scaleData: DbScaleData {}
    property int horizontalOrientation: DbScale.Orientation.Left
    property bool enableText: true
    property var backgroundColor: Qt.rgba(0, 0, 0, 1)

    onPaint: {
        var ctx = getContext("2d");
        ctx.fillStyle = backgroundColor;
        ctx.fillRect(0, 0, width, height);

        ctx.lineWidth = 1
        ctx.strokeStyle = Qt.rgba(1, 1, 1, 1)

        privates.drawLines(ctx)

        if (root.enableText) {
            ctx.font = "10px sans-serif"
            ctx.textAlign = horizontalOrientation === DbScale.Orientation.Left ? "left" : "right"
            privates.drawText(ctx)
        }
    }

    QtObject {
        id: privates

        function tickPos(pos) {
            return (1 - pos) * height
        }

        function drawLines(ctx) {
            var lineX = horizontalOrientation === DbScale.Orientation.Left ? 2 : root.width - 2
            var lineXE = horizontalOrientation === DbScale.Orientation.Left ? 10 : root.width - 10

            ctx.beginPath()

            ctx.moveTo(lineX, 0)
            ctx.lineTo(lineX, height)

            for (var i = 0; i < scaleData.ticks.length; ++i) {
                var t = scaleData.ticks[i]
                var yt = dbToPosition(t.db)

                drawTick(ctx, lineX, lineXE, yt)

                if (t.dbStep !== 0) {
                    console.assert(i !== scaleData.ticks.length)

                    for (var j = t.db + t.dbStep, je = scaleData.ticks[i + 1].db; j < je; j += t.dbStep) {
                        drawTick(ctx, lineX, lineXE, dbToPosition(j))
                    }
                }
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
            // then the text for every tick
            var textX = root.horizontalOrientation === DbScale.Orientation.Left ? root.width - 2 : root.width - 12

            for (var i = 0; i < scaleData.ticks.length; ++i) {
                var t = scaleData.ticks[i]
                var yt = dbToPosition(t.db)

                drawTickText(ctx, textX, yt, t.db)

                if (t.dbStep !== 0) {
                    console.assert(i !== scaleData.ticks.length)

                    for (var j = t.db + t.dbStep, je = scaleData.ticks[i + 1].db; j < je; j += t.dbStep) {
                        drawTickText(ctx, textX, dbToPosition(j), j)
                    }
                }
            }
        }

        function drawTickText(ctx, textX, yt, db) {
            ctx.strokeText(DbConvert.dbToString(db), textX, yt + 2)
        }

        function dbToPosition(db) {
            return tickPos(scaleData.dbToPosition(db))
        }
    }
}
