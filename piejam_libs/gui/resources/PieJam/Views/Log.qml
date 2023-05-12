// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ViewPane {

    id: root

    property alias logMessages: messages.model

    Frame {
        anchors.fill: parent
        anchors.margins: 4

        ListView {
            id: messages

            anchors.fill: parent

            clip: true

            delegate: Label {
                width: ListView.view.width

                text: modelData

                font.pixelSize: 18

                wrapMode: Text.WordWrap
            }
        }
    }
}
