// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

pragma Singleton

import QtQuick 2.15

import QtQml 2.15

Item {
    id: root

    property string logMessage: ""
    property string quickTip: ""

    readonly property string message: quickTipTimer.running ? quickTip : logMessage

    Timer {
        id: quickTipTimer

        interval: 2000
    }

    onQuickTipChanged: quickTipTimer.restart()
}
