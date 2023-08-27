// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import ".."

Item {
    id: root

    property string name: "X"
    property var activeParam: null
    property var channelParam: null

    implicitWidth: 164
    implicitHeight: 40

    RowLayout {

        anchors.fill: parent

        spacing: 4

        ParameterToggleButton {
            paramModel: root.activeParam

            Layout.preferredWidth: 32
            Layout.preferredHeight: 40

            text: root.name
            flat: true
        }

        EnumButtonGroup {
            paramModel: root.channelParam

            Layout.preferredWidth: 128
            Layout.preferredHeight: 40
        }
    }
}
