// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

pragma Singleton

import QtQuick 2.15

import QtQml 2.15

Item {
    id: root

    readonly property string message: quickTipTimer.running ? private_.quickTip : ""

    QtObject {
        id: private_

        property string quickTip: ""
    }

    Timer {
        id: quickTipTimer

        interval: 2000
    }

    function show(quickTip) {
        private_.quickTip = quickTip
        quickTipTimer.restart()
    }

    function showParameterValue(paramModel) {
        console.assert(paramModel)
        root.show("<b>" + paramModel.name + "</b>: " + paramModel.valueString)
    }
}
