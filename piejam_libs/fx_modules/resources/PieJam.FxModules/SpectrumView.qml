// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels
import PieJam.Util 1.0

import PieJam 1.0
import PieJam.FxChainControls 1.0

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    ColumnLayout {
        anchors.fill: parent

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            SpectrumGrid {
                anchors.fill: parent
            }

            PJItems.Spectrum {
                anchors.fill: parent

                spectrumData: root.model ? root.model.dataA : null
                color: Material.color(Material.Pink)
            }

            PJItems.Spectrum {
                anchors.fill: parent

                visible: root.model && root.model.busType === PJModels.Types.BusType.Stereo
                spectrumData: root.model ? root.model.dataB : null
                color: Material.color(Material.Blue)
            }
        }

        StreamSourceSettings {
            model: root.model

            Layout.fillWidth: true
            Layout.preferredHeight: 48
        }
    }

    onBypassedChanged: if (root.bypassed && root.model)
                           root.model.clear()
}
