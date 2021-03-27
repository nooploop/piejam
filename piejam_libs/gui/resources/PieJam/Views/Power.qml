// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ViewPane {
    id: root

    Column {
        width: 256
        height: 96

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            height: 24
            text: "Hint: Turn off you speakers first."
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {

            text: "Shutdown"
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: Qt.quit()
        }

    }
}
