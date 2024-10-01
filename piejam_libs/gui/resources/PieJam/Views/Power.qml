// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: root

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.preferredHeight: 24

            text: "Tip: Turn off you speakers first."
        }

        Button {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            text: "Shutdown"

            onClicked: Qt.quit()
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
