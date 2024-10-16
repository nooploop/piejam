// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import "../Controls"

Item {
    id: root

    property var model: null // TODO: currently subscribed by parent, possibly needs to be decoupled

    QtObject {
        id: private_

        readonly property bool isStereo: root.model && root.model.busType === PJModels.Types.BusType.Stereo
    }

    RowLayout {
        anchors.fill: parent

        spacing: 0

        StereoChannelSelector {
            visible: private_.isStereo

            name: "A"
            activeParam: root.model ? root.model.activeA : null
            channelParam: root.model ? root.model.channelA : null
            gainParam: root.model ? root.model.gainA : null

            Material.accent: root.model && root.model.activeA.value ? Material.Pink : Material.Grey
        }

        ToolSeparator {
            visible: private_.isStereo
        }

        StereoChannelSelector {
            visible: private_.isStereo

            name: "B"
            activeParam: root.model ? root.model.activeB : null
            channelParam: root.model ? root.model.channelB : null
            gainParam: root.model ? root.model.gainB : null

            Material.accent: root.model && root.model.activeB.value ? Material.Blue : Material.Grey
        }

        ParameterQuickSpinBox {
            visible: !private_.isStereo

            model: root.model ? root.model.gainA : null
            stepScale: 1.04167

            Layout.preferredWidth: 128
        }

        Item {
            Layout.preferredWidth: 497

            visible: !private_.isStereo
        }

        ToolSeparator {}

        ParameterToggleButton {
            model: root.model ? root.model.freeze : null

            Layout.preferredWidth: 34
            Layout.preferredHeight: 40

            flat: false

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/snow.svg"
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
