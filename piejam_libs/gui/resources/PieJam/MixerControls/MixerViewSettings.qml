// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

pragma Singleton

import QtQuick 2.0

Item {
    id: root

    readonly property int perform: 0
    readonly property int edit: 1
    readonly property int fx: 2
    readonly property int auxSend: 3

    property int mode: root.perform

    function switchMode(newMode) {
        root.mode = root.mode === newMode ? root.perform : newMode
    }
}
