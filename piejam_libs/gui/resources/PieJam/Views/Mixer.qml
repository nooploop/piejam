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

    ChannelsListView {
        id: inputs

        anchors.left: parent.left
        anchors.right: mainChannelStrip.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 8

        model: root.model.inputChannels

        soloActive: root.model.inputSoloActive

        onSoloToggled: root.model.setInputSolo(index)
        onFxButtonClicked: root.fxButtonClicked()
        onAddChannelClicked: root.model.addChannel(newChannelName)
    }

    Loader {
        id: mainChannelStrip

        anchors.right: mixerToolbar.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 8

        active: root.model.mainChannel !== null

        sourceComponent: ChannelStrip {

            perform.mono: root.model.mainChannel.mono
            perform.levelLeft: root.model.mainChannel.levelLeft
            perform.levelRight: root.model.mainChannel.levelRight
            perform.meterMuted: root.model.mainChannel.mute
            perform.pan: root.model.mainChannel.panBalance
            perform.volume: root.model.mainChannel.volume
            perform.mute: root.model.mainChannel.mute
            perform.solo: root.model.mainChannel.solo
            perform.name: root.model.mainChannel.name

            perform.volumeMidi.model: root.model.mainChannel.volumeMidi
            perform.panMidi.model: root.model.mainChannel.panMidi
            perform.muteMidi.model: root.model.mainChannel.muteMidi

            perform.onFaderMoved: root.model.mainChannel.changeVolume(newVolume)
            perform.onPanMoved: root.model.mainChannel.changePanBalance(newPan)
            perform.onMuteToggled: root.model.mainChannel.changeMute(!model.item.mute)
    //        perform.onSoloToggled: root.soloToggled(index)
            perform.onFxButtonClicked: {
                root.model.mainChannel.focusFxChain()
                root.fxButtonClicked()
            }

            edit.name: root.model.mainChannel.name
            edit.selectedInput: root.model.mainChannel.selectedInput
            edit.selectedInputIsValid: root.model.mainChannel.selectedInputIsValid
            edit.inputDevices: root.model.mainChannel.inputDevices
            edit.inputChannels: root.model.mainChannel.inputChannels
            edit.selectedOutput: root.model.mainChannel.selectedOutput
            edit.selectedOutputIsValid: root.model.mainChannel.selectedOutputIsValid
            edit.outputDevices: root.model.mainChannel.outputDevices
            edit.outputChannels: root.model.mainChannel.outputChannels

            edit.onAudioInMixSelected: root.model.mainChannel.changeInputToMix()
            edit.onAudioInDeviceSelected: root.model.mainChannel.changeInputToDevice(index)
            edit.onAudioInChannelSelected: root.model.mainChannel.changeInputToChannel(index)
            edit.onAudioOutNoneSelected: root.model.mainChannel.changeOutputToNone()
            edit.onAudioOutDeviceSelected: root.model.mainChannel.changeOutputToDevice(index)
            edit.onAudioOutChannelSelected: root.model.mainChannel.changeOutputToChannel(index)
            edit.onNameEdited: root.model.mainChannel.changeName(newName)
            edit.deletable: false

            Binding {
                target: root.model.mainChannel
                property: "subscribed"
                value: visible
            }
        }

    }


    Rectangle {
        id: mixerToolbar

        width: 48

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        color: Material.color(Material.Grey, Material.Shade800)

        ButtonGroup {
            id: mixerViewButtonGroup
        }

        Button {
            id: performButton

            width: 32
            height: 44

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 4

            ButtonGroup.group: mixerViewButtonGroup

            text: "P"
            checkable: true
            checked: MixerViewSettings.mode === MixerViewSettings.perform
            autoExclusive: true

            onClicked: MixerViewSettings.mode = MixerViewSettings.perform
        }

        Button {
            id: editButton

            width: 32
            height: 44

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 4

            ButtonGroup.group: mixerViewButtonGroup

            text: "E"
            checkable: true
            checked: MixerViewSettings.mode === MixerViewSettings.edit
            autoExclusive: true

            onClicked: MixerViewSettings.mode = MixerViewSettings.edit
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
