// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real posX: 0
    property real posY: 0

    signal posChanged(real x, real y)

    MouseArea {
        id: mouseArea

        readonly property real xMax: mouseArea.width - 1
        readonly property real yMax: mouseArea.height - 1

        anchors.fill: parent
        anchors.margins: 8

        preventStealing: true

        function changePos(mouseX, mouseY) {
            handle.x = MathExt.clamp(mouseX, 0, mouseArea.xMax)
            handle.y = MathExt.clamp(mouseY, 0, mouseArea.yMax)

            root.posChanged(MathExt.toNormalized(handle.x, 0, mouseArea.xMax),
                            MathExt.toNormalized(handle.y, 0, mouseArea.yMax))
        }

        onPressed: {
            changePos(mouse.x, mouse.y)

            mouse.accepted = true
        }

        onPositionChanged: changePos(mouse.x, mouse.y)

        onXMaxChanged: handle.x = MathExt.fromNormalized(root.posX, 0, mouseArea.xMax)
        onYMaxChanged: handle.y = MathExt.fromNormalized(root.posY, 0, mouseArea.yMax)
    }

    onPosXChanged: handle.x = MathExt.fromNormalized(root.posX, 0, mouseArea.xMax)
    onPosYChanged: handle.y = MathExt.fromNormalized(root.posY, 0, mouseArea.yMax)

    Rectangle {
        id: handle

        width: 16
        height: 16

        radius: 8

        color: Material.color(Material.Yellow, Material.Shade400)
    }
}
