// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

pragma Singleton

import QtQuick 2.0

Item {
    id: root

    readonly property int perform: 0
    readonly property int edit: 1

    default property int mode: root.perform
}
