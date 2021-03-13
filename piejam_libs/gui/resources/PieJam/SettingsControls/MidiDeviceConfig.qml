// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property alias name: nameLabel.text
    property alias deviceEnabled: enableSwitch.checked

    signal enabledToggled(bool newEnabled)

    implicitWidth: 192
    implicitHeight: 64

    Switch {
        id: enableSwitch

        width: 64
        height: 32

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left

        onClicked: enabledToggled(enableSwitch.checked)
    }

    Label {
        id: nameLabel

        height: 32

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: enableSwitch.right
        anchors.right: parent.right

        verticalAlignment: Text.AlignVCenter
        textFormat: Text.PlainText
        font.bold: true
        font.pixelSize: 16
    }
}
