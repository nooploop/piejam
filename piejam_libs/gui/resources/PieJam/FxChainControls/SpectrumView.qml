// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels

import ".."

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    ColumnLayout {
        anchors.fill: parent

        PJItems.Spectrum {
            id: spectrum

            Layout.fillWidth: true
            Layout.fillHeight: true

            spectrumAData: root.model ? root.model.dataA : null
            spectrumAColor: Material.color(Material.Pink)

            spectrumBData: root.model ? root.model.dataB : null
            spectrumBColor: Material.color(Material.Blue)
        }

        StreamSourceSettings {
            model: root.model

            Layout.fillWidth: true
            Layout.preferredHeight: 48
        }
    }

    onBypassedChanged: if (root.bypassed && root.model)
                           root.model.clear()

    Repeater {
        model: spectrum.levelLabels

        delegate: Label {
            x: spectrum.width - implicitWidth - 4
            y: modelData.position

            font.pointSize: 6

            text: modelData.value + " dB"
        }
    }

    Repeater {
        model: spectrum.frequencyLabels

        delegate: Label {
            x: modelData.position + 2
            y: spectrum.height - implicitHeight

            font.pointSize: 6

            text: modelData.value >= 1000 ? (modelData.value / 1000)
                                            + " kHz" : modelData.value + " Hz"
        }
    }
}
