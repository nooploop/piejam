// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"

SubscribableItem {
    id: root

    implicitWidth: 132

    Frame {
        id: frame

        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            HeaderLabel {
                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? root.model.name : ""
            }

            ParameterSymmetricBipolarSlider {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                model: root.model ? root.model.panBalance : null
            }

            LevelMeterVolumeFader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                volume: root.model ? root.model.volume : null
                peakLevel: root.model ? root.model.peakLevel : null
                rmsLevel: root.model ? root.model.rmsLevel : null

                muted: root.model && (root.model.mute.value || root.model.mutedBySolo)

                levelMeterScale: PJModels.MixerDbScales.levelMeterScale
                volumeFaderScale: PJModels.MixerDbScales.volumeFaderScale
            }

            ChannelControls {
                Layout.fillWidth: true
                Layout.preferredHeight: 40

                record: root.model ? root.model.record : null
                mute: root.model ? root.model.mute : null
                solo: root.model ? root.model.solo : null
            }
        }
    }
}
