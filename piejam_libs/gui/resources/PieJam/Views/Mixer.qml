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

            perform.model: root.model.mainChannel.perform
            perform.onFxButtonClicked: root.fxButtonClicked()

            edit.model: root.model.mainChannel.edit
            edit.deletable: false

//    //        perform.onSoloToggled: root.soloToggled(index)
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
