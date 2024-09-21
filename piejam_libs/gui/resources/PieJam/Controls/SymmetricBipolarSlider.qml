// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../Util/ColorExt.js" as ColorExt
import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property alias value: slider.value

    signal moved(real newValue)

    Slider {
        id: slider

        anchors.fill: parent

        from: -1
        to: 1

        background: Rectangle {
            height: 4

            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.verticalCenter: parent.verticalCenter

            color: ColorExt.setAlpha(Material.accentColor, 0.25)

            Rectangle {
                readonly property real halfParentWidth: parent.width / 2

                x: slider.value < 0 ? ((1 + slider.value) * halfParentWidth) : halfParentWidth

                height: parent.height
                width: halfParentWidth * Math.abs(slider.value)

                color: Material.accentColor
            }
        }

        onMoved: root.moved(slider.value)
    }
}
