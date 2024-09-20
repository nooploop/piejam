// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real value: 0
    property real resolution: 1000

    signal changeValue(real newValue)

    Rectangle {
        id: touchBackground

        anchors.fill: parent

        color: Material.backgroundColor
        border.width: 1
        border.color: Material.backgroundDimColor
        radius: 2
    }

    MouseArea {
        id: touchArea

        property real inc: 1 / root.resolution
        property real lastY: 0

        anchors.fill: parent

        preventStealing: true

        onPressed: {
            lastY = mouse.y
            mouse.accepted = true
        }

        onPositionChanged: {
            root.changeValue(MathExt.clamp(root.value + (lastY - mouse.y) * inc, 0, 1))
            lastY = mouse.y
        }
    }
}
