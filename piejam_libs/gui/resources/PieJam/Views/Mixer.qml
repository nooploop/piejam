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

        ChannelsListView {
            id: inputs

            SplitView.fillWidth: true
            SplitView.minimumWidth: 150

            model: root.model.inputChannels

            soloActive: root.model.inputSoloActive

            onSoloToggled: root.model.setInputSolo(index)
            onFxButtonClicked: root.fxButtonClicked()
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
