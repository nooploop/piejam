// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real posX: 0
    property real posY: 0

    signal posChanged(real x, real y)

    QtObject {
        id: internal

        function fromNormalized(v, wh)
        {
            return MathExt.mapTo(MathExt.clamp(v, 0, 1), 0, 1, 0, wh - 1)
        }

        function toNormalized(v, wh)
        {
            return MathExt.mapTo(MathExt.clamp(v, 0, wh - 1), 0, wh - 1, 0, 1);
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        anchors.margins: 8

        preventStealing: true

        onPressed: {
            handle.x = MathExt.clamp(mouse.x, 0, mouseArea.width - 1)
            handle.y = MathExt.clamp(mouse.y, 0, mouseArea.height - 1)

            root.posChanged(internal.toNormalized(handle.x, mouseArea.width),
                            internal.toNormalized(handle.y, mouseArea.height))

            mouse.accepted = true
        }

        onPositionChanged: {
            handle.x = MathExt.clamp(mouse.x, 0, mouseArea.width - 1)
            handle.y = MathExt.clamp(mouse.y, 0, mouseArea.height - 1)

            root.posChanged(internal.toNormalized(handle.x, mouseArea.width),
                            internal.toNormalized(handle.y, mouseArea.height))
        }

        onWidthChanged: handle.x = internal.fromNormalized(root.posX, mouseArea.width)
        onHeightChanged: handle.y = internal.fromNormalized(root.posY, mouseArea.height)
    }

    onPosXChanged: handle.x = internal.fromNormalized(root.posX, mouseArea.width)
    onPosYChanged: handle.y = internal.fromNormalized(root.posY, mouseArea.height)

    Rectangle {
        id: handle

        width: 16
        height: 16

        radius: 8

        color: Qt.rgba(0, 0, 0, 0)

        border.color: Material.color(Material.Yellow, Material.Shade400)
        border.width: 2
    }
}
