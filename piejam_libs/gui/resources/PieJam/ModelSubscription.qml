// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import QtQml 2.15

Item {
    id: root

    property var target
    property bool subscribed: false

    Binding {
        id: binding

        when: root.target
        target: root.target
        property: "subscribed"
        value: root.subscribed && !Qt.inputMethod.visible
        restoreMode: Binding.RestoreBinding
    }
}
