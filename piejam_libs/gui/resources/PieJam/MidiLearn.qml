// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

pragma Singleton

import QtQuick 2.0

Item {
    id: root

    property bool active: false
    readonly property bool learning: privates.learningControl !== null

    function start(ctrl) {
        console.assert(ctrl !== null)

        stop()
        privates.learningControl = ctrl
        privates.learningControl.startLearn()
    }

    function stop() {
        if (privates.learningControl !== null) {
            privates.learningControl.stopLearn()
            privates.learningControl = null
        }
    }

    onActiveChanged: if (!active) stop()

    QtObject {
        id: privates

        property MidiAssignable learningControl: null
    }
}
