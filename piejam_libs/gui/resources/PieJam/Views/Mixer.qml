// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import ".."
import "../MixerControls"

TopPane {
    id: root

    property var model

    signal fxButtonClicked()

    SplitView {
        anchors.fill: parent
        anchors.margins: 8

        ListView {
            id: inputs

            SplitView.fillWidth: true
            SplitView.minimumWidth: 150

            spacing: 2
            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            reuseItems: true

            model: root.model.inputChannels

            delegate: ChannelStrip {
                id: inputChannelStrip

                anchors.top: if (parent) parent.top
                anchors.bottom: if (parent) parent.bottom

                mono: model.item.mono
                levelLeft: model.item.levelLeft
                levelRight: model.item.levelRight
                meterMuted: (model.item.mute && !model.item.solo) || (root.model.inputSoloActive && !model.item.solo)
                pan: model.item.panBalance
                volume: model.item.volume
                mute: model.item.mute
                solo: model.item.solo
                name: model.item.name

                volumeMidi.model: model.item.volumeMidi
                panMidi.model: model.item.panMidi
                muteMidi.model: model.item.muteMidi

                onFaderMoved: model.item.changeVolume(newVolume)
                onPanMoved: model.item.changePanBalance(newPan)
                onMuteToggled: model.item.changeMute(!model.item.mute)
                onSoloToggled: root.model.setInputSolo(index)
                onFxButtonClicked: {
                    model.item.focusFxChain()
                    root.fxButtonClicked()
                }

                Binding {
                    target: model.item
                    property: "subscribed"
                    value: visible
                }
            }
        }

        ListView {
            id: outputs

            SplitView.minimumWidth: 150

            spacing: 2
            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            reuseItems: true

            model: root.model.outputChannels

            delegate: ChannelStrip {
                id: outputChannelStrip

                anchors.top: if (parent) parent.top
                anchors.bottom: if (parent) parent.bottom

                levelLeft: model.item.levelLeft
                levelRight: model.item.levelRight
                meterMuted: model.item.mute
                pan: model.item.panBalance
                volume: model.item.volume
                mute: model.item.mute
                soloEnabled: false
                name: model.item.name

                volumeMidi.model: model.item.volumeMidi
                panMidi.model: model.item.panMidi
                muteMidi.model: model.item.muteMidi

                onFaderMoved: model.item.changeVolume(newVolume)
                onPanMoved: model.item.changePanBalance(newPan)
                onMuteToggled: model.item.changeMute(!model.item.mute)
                onFxButtonClicked: {
                    model.item.focusFxChain()
                    root.fxButtonClicked()
                }

                Binding {
                    target: model.item
                    property: "subscribed"
                    value: visible
                }
            }
        }
    }

    Timer {
        interval: 16
        running: root.visible
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
