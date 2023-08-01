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

Item {
    id: root

    property var model

    implicitWidth: 132

    Frame {
        id: frame

        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            HeaderLabel {
                id: title

                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? root.model.name : ""
            }

            BipolarSlider {
                id: panControls

                Layout.fillWidth: true
                Layout.preferredHeight: 40

                value: root.model ? root.model.panBalance : 0

                onMoved: {
                    root.model.changePanBalance(panControls.value)
                    Info.quickTip = (root.model.busType
                                     == PJModels.BusType.Mono ? "<b>Pan:</b> " : "<b>Balance:</b> ")
                            + panControls.value.toFixed(2)
                }

                MidiAssignArea {
                    id: midiAssignPan

                    model: root.model ? root.model.panMidi : null

                    anchors.fill: parent
                    anchors.topMargin: 1
                    anchors.bottomMargin: 1
                }
            }

            LevelMeterVolumeFader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                volume: root.model ? root.model.volume : 0
                levelLeft: root.model ? root.model.levelLeft : 0
                levelRight: root.model ? root.model.levelRight : 0

                muted: root.model ? (root.model.mute
                                     || root.model.mutedBySolo) : false

                volumeMidi: root.model ? root.model.volumeMidi : null

                levelMeterScale: PJModels.MixerDbScales.levelMeterScale
                volumeFaderScale: PJModels.MixerDbScales.volumeFaderScale

                onFaderMoved: root.model.changeVolume(newVolume)
            }

            ChannelControls {
                id: channelControls

                Layout.fillWidth: true
                Layout.preferredHeight: 32

                record: root.model ? root.model.record : false
                mute: root.model ? root.model.mute : false
                solo: root.model ? root.model.solo : false

                muteMidi: root.model ? root.model.muteMidi : null
                soloMidi: root.model ? root.model.soloMidi : null

                onRecordToggled: root.model.changeRecord(!root.model.record)
                onMuteToggled: root.model.changeMute(!root.model.mute)
                onSoloToggled: root.model.changeSolo(!root.model.solo)
            }
        }
    }

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }
}
