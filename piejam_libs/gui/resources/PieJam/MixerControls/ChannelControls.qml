// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property alias record: recordButton.model
    property alias solo: soloButton.model
    property alias mute: muteButton.model

    RowLayout {
        anchors.fill: parent

        spacing: 6

        ParameterToggleButton {
            id: recordButton

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("R")
            Material.accent: Material.Red
        }

        ParameterToggleButton {
            id: soloButton

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("S")
            Material.accent: Material.Yellow
        }

        ParameterToggleButton {
            id: muteButton

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("M")
        }
    }
}
