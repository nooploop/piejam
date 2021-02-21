// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ListView {
    id: root

    property bool soloActive: false

    signal soloToggled(int index)
    signal fxButtonClicked()
    signal addChannelClicked()

    spacing: 2
    clip: true
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    boundsMovement: Flickable.StopAtBounds
    reuseItems: true

    delegate: ChannelStrip {
        anchors.top: if (parent) parent.top
        anchors.bottom: if (parent) parent.bottom

        perform.mono: model.item.mono
        perform.levelLeft: model.item.levelLeft
        perform.levelRight: model.item.levelRight
        perform.meterMuted: (model.item.mute && !model.item.solo) || (root.soloActive && !model.item.solo)
        perform.pan: model.item.panBalance
        perform.volume: model.item.volume
        perform.mute: model.item.mute
        perform.solo: model.item.solo
        perform.name: model.item.name

        perform.volumeMidi.model: model.item.volumeMidi
        perform.panMidi.model: model.item.panMidi
        perform.muteMidi.model: model.item.muteMidi

        perform.onFaderMoved: model.item.changeVolume(newVolume)
        perform.onPanMoved: model.item.changePanBalance(newPan)
        perform.onMuteToggled: model.item.changeMute(!model.item.mute)
        perform.onSoloToggled: root.soloToggled(index)
        perform.onFxButtonClicked: {
            model.item.focusFxChain()
            root.fxButtonClicked()
        }

        edit.name: model.item.name
        edit.inputDevices: model.item.inputDevices
        edit.inputChannels: model.item.inputChannels
        edit.outputDevices: model.item.outputDevices
        edit.outputChannels: model.item.outputChannels

        edit.onAudioInMixSelected: model.item.changeInputToMix()
        edit.onAudioInDeviceSelected: model.item.changeInputToDevice(index)
        edit.onAudioInChannelSelected: model.item.changeInputToChannel(index)
        edit.onAudioOutNoneSelected: model.item.changeOutputToNone()
        edit.onAudioOutDeviceSelected: model.item.changeOutputToDevice(index)
        edit.onAudioOutChannelSelected: model.item.changeOutputToChannel(index)
        edit.onNameEdited: model.item.changeName(newName)
        edit.onDeleteClicked: model.item.deleteChannel()

        Binding {
            target: model.item
            property: "subscribed"
            value: visible
        }
    }

    footer: Item {
        width: 152

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        visible: MixerViewSettings.mode == MixerViewSettings.edit

        ChannelAddStrip {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            onAddClicked: root.addChannelClicked()
        }
    }
}

