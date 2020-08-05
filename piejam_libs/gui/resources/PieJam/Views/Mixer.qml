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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import QtQml.Models 2.12

import "../MixerControls"

TopPane {
    id: root

    property var model

    ListView {
        id: inputs

        anchors.right: outputLevelMeterFader.left
        anchors.rightMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8
        spacing: 2
        clip: true
        orientation: ListView.Horizontal

        boundsBehavior: Flickable.StopAtBounds

        model: root.model.inputChannels

        delegate: ChannelStrip {
            id: inputChannelStrip

            height: outputLevelMeterFader.height

            levelLeft: modelData.levelLeft
            levelRight: modelData.levelRight
            pan: modelData.panBalance
            gain: modelData.gain
            name: modelData.name

            onFaderMoved: root.model.setInputChannelGain(index, newGain)
            onPanMoved: root.model.setInputChannelPan(index, inputChannelStrip.pan)
        }
    }

    ChannelStrip {
        id: outputLevelMeterFader

        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8

        name: "Main"
        levelLeft: root.model.outputChannel.levelLeft
        levelRight: root.model.outputChannel.levelRight
        pan: root.model.outputChannel.panBalance
        gain: root.model.outputChannel.gain

        onFaderMoved: root.model.setOutputChannelGain(newGain)
        onPanMoved: root.model.setOutputChannelBalance(outputLevelMeterFader.pan)
    }

    Timer {
        interval: 16
        running: root.visible
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }
}
