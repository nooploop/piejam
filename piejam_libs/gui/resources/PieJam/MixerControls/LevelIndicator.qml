// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13

Item {
    id: root

    property real level: 1
    property alias gradient: backgroundRect.gradient
    property alias fillColor: filler.color

    width: 40
    height: 200

    Rectangle {
        id: backgroundRect

        border.width: 1
        anchors.fill: parent
    }

    Rectangle {
        id: filler

        x: 0
        y: 0
        width: root.width
        height: (1 - level) * parent.height
    }
}
