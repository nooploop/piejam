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
        onAddChannelClicked: root.model.addChannel()
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
    //        perform.onFxButtonClicked: {
    //            model.item.focusFxChain()
    //            root.fxButtonClicked()
    //        }

            edit.deletable: false
            perform.onFxButtonClicked: root.fxButtonClicked()

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
            buttons: toolbarButtons.children
        }

        Column {
            id: toolbarButtons
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: editButton

                width: 32
                height: 44

                text: "E"
                checkable: true
                checked: MixerViewSettings.mode === MixerViewSettings.edit
                autoExclusive: true

                onClicked: MixerViewSettings.mode = MixerViewSettings.edit
            }

            Button {
                id: performButton

                width: 32
                height: 44

                text: "P"
                checkable: true
                checked: MixerViewSettings.mode === MixerViewSettings.perform
                autoExclusive: true

                onClicked: MixerViewSettings.mode = MixerViewSettings.perform
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
