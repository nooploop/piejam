// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

import ".."

Pane {
    id: root

    property var model: null

    padding: 0

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }
}
