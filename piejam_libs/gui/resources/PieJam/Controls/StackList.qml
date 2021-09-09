// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

StackLayout {
    id: root

    property alias model: creator.model
    property alias delegate: creator.delegate

    Repeater {
        id: creator

        anchors.fill: parent
    }
}
