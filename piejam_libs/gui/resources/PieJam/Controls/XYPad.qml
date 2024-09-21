// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real posX: 0
    property real posY: 0
    property alias handleRadius: handle.radius
    property alias handleColor: handle.color

    signal changePos(real x, real y)

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        anchors.margins: 0

        preventStealing: true

        function changePos(mouseX, mouseY) {
            handle.x = MathExt.clamp(mouseX, 0, mouseArea.width)
            handle.y = MathExt.clamp(mouseY, 0, mouseArea.height)

            root.changePos(MathExt.toNormalized(handle.x, 0, mouseArea.width),
                           MathExt.toNormalized(handle.y, 0, mouseArea.height))
        }

        onPressed: {
            changePos(mouse.x, mouse.y)

            mouse.accepted = true
        }

        onPositionChanged: mouseArea.changePos(mouse.x, mouse.y)

        onWidthChanged: handle.x = MathExt.fromNormalized(root.posX, 0, mouseArea.width)
        onHeightChanged: handle.y = MathExt.fromNormalized(root.posY, 0, mouseArea.height)
    }

    onPosXChanged: handle.x = MathExt.fromNormalized(root.posX, 0, mouseArea.width)
    onPosYChanged: handle.y = MathExt.fromNormalized(root.posY, 0, mouseArea.height)

    Circle {
        id: handle
    }
}
