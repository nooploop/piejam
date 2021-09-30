// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

Item {
    id: root

    property alias radius: rect.radius
    property alias color: rect.color

    Rectangle {
        id: rect

        x: -radius
        y: -radius

        width: radius * 2
        height: radius * 2
    }
}
