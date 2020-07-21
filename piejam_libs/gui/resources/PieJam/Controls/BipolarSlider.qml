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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Controls/Color.js" as ColorExt
import "../Util/MathExt.js" as MathExt

Item {
    property alias value: slider.value

    signal moved()

    id: root

    Slider {
        id: slider
        anchors.fill: parent

        from: -1
        to: 1

        background: Item {
            id: backItem

            Rectangle {
                height: 4
                color: ColorExt.setAlpha(Material.accentColor, 0.25)
                anchors.rightMargin: 8
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.left: parent.left

                Rectangle {
                    color: Material.accentColor
                    x: slider.value < 0 ? ((1 + slider.value) * (parent.width / 2)) : (parent.width / 2)
                    height: parent.height
                    width: (parent.width / 2) * Math.abs(slider.value)
                }
            }
        }

        onMoved: root.moved()
    }
}
