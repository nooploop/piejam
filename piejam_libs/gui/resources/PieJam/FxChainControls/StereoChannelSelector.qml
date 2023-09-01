// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import "../Controls"

Item {
    id: root

    property string name: "X"
    property var activeParam: null
    property var channelParam: null
    property var gainParam: null

    implicitWidth: 256
    implicitHeight: 40

    RowLayout {

        anchors.fill: parent

        spacing: 8

        ParameterToggleButton {
            model: root.activeParam

            Layout.preferredWidth: 32
            Layout.fillHeight: true

            text: root.name
            flat: true
        }

        EnumButtonGroup {
            model: root.channelParam

            Layout.preferredWidth: 128
            Layout.fillHeight: true
        }

        ParameterQuickSpinBox {
            model: root.gainParam
            stepScale: 1.04167

            Layout.preferredWidth: 128
            Layout.fillHeight: true
        }
    }
}
