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
    implicitHeight: 48

    Label {
        id: nameLabel

        height: 32

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: enableSwitch.left
        anchors.top: parent.top
        anchors.topMargin: 8

        verticalAlignment: Text.AlignVCenter
        font.bold: true
        font.pixelSize: 16
    }

    Switch {
        id: enableSwitch

        width: 72
        height: 32

        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8

        onClicked: enabledToggled(enableSwitch.checked)
    }
}
