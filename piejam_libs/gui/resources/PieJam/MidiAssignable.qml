// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0

Item {
    id: root

    signal learningStarted()
    signal learningStopped()

    function startLearn() {
        root.learningStarted()
    }

    function stopLearn() {
        root.learningStopped()
    }
}
