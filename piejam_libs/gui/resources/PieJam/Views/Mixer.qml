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
        id: inputToggles

        anchors.top: parent.top
        anchors.left: inputsLabel.right
        anchors.right: parent.right
        orientation: ListView.Horizontal

        model: root.model.inputChannels

        delegate: CheckDelegate {
            property bool modelEnabled: model.enabled

            id: checkDelegate

            text: index + 1
            checkState: model.enabled ? Qt.Checked : Qt.Unchecked
            height: 40
            width: 90

            onClicked: root.model.toggleInputChannel(index)

            Connections {
                target: checkDelegate
                function onModelEnabledChanged() { inputFadersVisualModel.items.get(index).inDisplayed = model.enabled }
            }
        }
    }

    Label {
        id: inputsLabel
        width: 120
        height: 40
        text: qsTr("Inputs:")
        font.bold: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        visible: root.model.inputChannels.length !== 0
    }

    DelegateModel {
        id: inputFadersVisualModel

        model: root.model.inputChannels

        groups: [
            DelegateModelGroup {
                name: "displayed"
                includeByDefault: false
            }
        ]

        filterOnGroup: "displayed"

        delegate: Loader {
            asynchronous: root.visible
            sourceComponent: LevelMeterFader {
                height: 384
                mono: model.mono
                level: model.level
                levelRight: model.levelRight
                gain: model.gain
                name: "In " + (index + 1)

                onFaderMoved: root.model.setInputChannelGain(index, newGain)
            }
        }

        Component.onCompleted: updateDisplayedChannels()

        function updateDisplayedChannels() {
            for (var i = 0; i < items.count; ++i) {
                var entry = items.get(i)
                entry.inDisplayed = entry.model.enabled
            }
        }
    }

    ListView {
        id: inputs
        x: 0
        y: 48
        width: 640
        height: 384
        spacing: 2
        clip: true
        orientation: ListView.Horizontal

        boundsBehavior: Flickable.StopAtBounds

        model: inputFadersVisualModel
    }

    LevelMeterFader {
        id: outputLevelMeterFader
        y: 48
        height: 384
        anchors.right: parent.right

        name: "Master"
        mono: root.model.outputChannel.mono
        level: root.model.outputChannel.level
        levelRight: root.model.outputChannel.levelRight
        gain: root.model.outputChannel.gain

        onFaderMoved: root.model.setOutputChannelGain(newGain)
    }

    Timer {
        interval: 16
        running: root.visible
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }

    Connections {
        target: root.model
        function onInputChannelsChanged() {
            inputFadersVisualModel.model = root.model.inputChannels
            inputFadersVisualModel.updateDisplayedChannels()
        }
    }
}
