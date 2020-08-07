// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../MixerControls"

TopPane {
    id: root

    property var model

    SplitView {
        anchors.fill: parent
        anchors.margins: 8

        ListView {
            id: inputs

            SplitView.fillWidth: true
            SplitView.minimumWidth: 142

            spacing: 2
            clip: true
            orientation: ListView.Horizontal

            boundsBehavior: Flickable.StopAtBounds

            model: root.model.inputChannels

            delegate: ChannelStrip {
                id: inputChannelStrip

                anchors.top: parent.top
                anchors.bottom: parent.bottom

                levelLeft: modelData.levelLeft
                levelRight: modelData.levelRight
                pan: modelData.panBalance
                gain: modelData.gain
                name: modelData.name

                onFaderMoved: root.model.setInputChannelGain(index, newGain)
                onPanMoved: root.model.setInputChannelPan(index, inputChannelStrip.pan)
            }
        }

        ListView {
            id: outputs

            SplitView.minimumWidth: 142

            spacing: 2
            clip: true
            orientation: ListView.Horizontal

            boundsBehavior: Flickable.StopAtBounds

            model: root.model.outputChannels

            delegate: ChannelStrip {
                id: outputChannelStrip

                anchors.top: parent.top
                anchors.bottom: parent.bottom

                levelLeft: modelData.levelLeft
                levelRight: modelData.levelRight
                pan: modelData.panBalance
                gain: modelData.gain
                name: modelData.name

                onFaderMoved: root.model.setOutputChannelGain(index, newGain)
                onPanMoved: root.model.setOutputChannelPan(index, outputChannelStrip.pan)
            }
        }
    }

    Timer {
        interval: 16
        running: root.visible
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }
}
