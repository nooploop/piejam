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
        anchors.left: parent.left
        anchors.right: mixerToolbar.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 8

        ChannelsListView {
            id: inputs

            SplitView.fillWidth: true
            SplitView.minimumWidth: 150

            model: root.model.inputChannels

            soloActive: root.model.inputSoloActive

            onSoloToggled: root.model.setInputSolo(index)
            onFxButtonClicked: root.fxButtonClicked()
            onAddChannelClicked: root.model.addChannel()
        }

        ChannelsListView {
            id: outputs

            SplitView.minimumWidth: 150

            model: root.model.outputChannels

            soloActive: root.model.outputSoloActive

            onSoloToggled: root.model.setOutputSolo(index)
            onFxButtonClicked: root.fxButtonClicked()
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
