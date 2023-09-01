// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property var record: null
    property var solo: null
    property var mute: null

    RowLayout {
        anchors.fill: parent

        spacing: 6

        ParameterToggleButton {
            model: root.record

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("R")
            Material.accent: Material.Red
        }

        ParameterToggleButton {
            model: root.solo

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("S")
            Material.accent: Material.Yellow
        }

        ParameterToggleButton {
            model: root.mute

            Layout.fillWidth: true
            Layout.preferredHeight: 40

            text: qsTr("M")
        }
    }
}
