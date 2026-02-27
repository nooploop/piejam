// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias name: nameLabel.text
    property alias checked: toggle.checked

    signal toggled(bool value)

    implicitWidth: 300
    implicitHeight: 64

    Frame {
        id: frame

        anchors.fill: parent

        spacing: 0

        RowLayout {
            anchors.fill: parent

            Label {
                id: nameLabel

                Layout.fillWidth: true

                textFormat: Text.PlainText
                font.pixelSize: 18
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Switch {
                id: toggle

                onToggled: root.toggled(checked)
            }
        }
    }
}
