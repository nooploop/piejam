// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13

TopPane {
    id: root

    Button {
        x: 350
        y: 170
        text: "Shutdown"

        onClicked: Qt.quit()
    }
}
