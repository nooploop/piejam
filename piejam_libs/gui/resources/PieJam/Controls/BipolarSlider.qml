// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

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
