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
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../Util/MathExt.js" as MathExt

Item {
    property bool bipolar: true
    property alias value: dial.value

    signal moved()

    id: root

    Dial {
        id: dial

        width: Math.min(root.width, root.height)
        height: dial.width
        anchors.horizontalCenter: parent.horizontalCenter

        inputMode: Dial.Vertical
        from: root.bipolar ? -1 : 0

        background: Canvas {
            id: knobRing

            onPaint: {
                var ctx = getContext("2d")
                var centerX = dial.width / 2
                var centerY = dial.width / 2
                var startAngle = MathExt.toRad(root.bipolar ? 270 : 140)
                var endAngle = MathExt.toRad(MathExt.mapTo(dial.angle, -140, 140, 140, 400))
                var direction = root.bipolar && dial.angle < 0

                ctx.fillStyle = Material.backgroundColor
                ctx.fillRect(0, 0, dial.width, dial.height)

                ctx.beginPath()
                ctx.lineWidth = 2
                ctx.strokeStyle = Material.accentColor
                ctx.ellipse(1, 1, dial.width - 2, dial.width - 2)
                ctx.closePath()
                ctx.stroke()

                ctx.beginPath()
                ctx.lineWidth = 4
                ctx.strokeStyle = Material.accentColor
                ctx.arc(centerX, centerY, centerX - 3, startAngle, endAngle, direction)
                ctx.moveTo((centerX - 1) * Math.cos(endAngle) + centerX, (centerX - 1) * Math.sin(endAngle) + centerX)
                ctx.lineTo(centerX, centerY)
                ctx.moveTo(0, 0)
                ctx.closePath()
                ctx.stroke()
            }
        }

        handle: Item {}

        onMoved: {
            root.moved()
            knobRing.requestPaint()
        }
    }

}
