// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import ".."
import "../MixerControls"

ViewPane {
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
        }

    }


    Rectangle {
        id: mixerToolbar

        width: 48

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        color: Material.color(Material.Grey, Material.Shade800)

        Button {
            id: editButton

            width: 32
            height: 44

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 4

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/pencil.svg"
            display: AbstractButton.IconOnly

            checkable: true
            checked: MixerViewSettings.mode === MixerViewSettings.edit

            onClicked: {
                if (MixerViewSettings.mode === MixerViewSettings.edit)
                    MixerViewSettings.mode = MixerViewSettings.perform
                else
                    MixerViewSettings.mode = MixerViewSettings.edit
            }
        }
    }

    Timer {
        interval: 16
        running: root.visible && MixerViewSettings.mode === MixerViewSettings.perform
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
