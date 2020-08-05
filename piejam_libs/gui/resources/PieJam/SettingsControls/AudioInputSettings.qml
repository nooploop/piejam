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

Item {

    property variant model: {
        "channels": [ "-", 1, 2, 3, 4, 5 ],
        "busConfigs": [
            { "name": "In 1 (M)", "mono": true, "monoChannel": 1 },
            { "name": "In 2 (S)", "mono": false, "stereoLeftChannel": 2, "stereoRightChannel": 3 }
        ],
        "selectMonoChannel": function (idx, ch) { console.log("selectMonoChannel", idx, ch) },
        "selectStereoLeftChannel": function (idx, ch) { console.log("selectStereoLeftChannel", idx, ch) },
        "selectStereoRightChannel": function (idx, ch) { console.log("selectStereoRightChannel", idx, ch) },
        "deleteBus": function (idx) { console.log("deleteBus", idx) },
        "addMonoBus": function () { console.log("addMonoBus") }
    }

    id: root

    ListView {
        id: inputs

        model: root.model.busConfigs

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: addMono.top
        anchors.margins: 8

        spacing: 8

        clip: true

        delegate: BusConfig {
            height: 64
            name: modelData.name
            mono: modelData.mono
            channels: root.model.channels
            monoChannelIndex: modelData.mono ? modelData.monoChannel : 0
            stereoLeftChannelIndex: modelData.mono ? 0 : modelData.stereoLeftChannel
            stereoRightChannelIndex: modelData.mono ? 0 : modelData.stereoRightChannel

            anchors.left: parent.left
            anchors.right: parent.right

            onMonoChannelSelected: root.model.selectMonoChannel(index, ch)
            onStereoLeftChannelSelected: root.model.selectStereoLeftChannel(index, ch)
            onStereoRightChannelSelected: root.model.selectStereoRightChannel(index, ch)
            onDeleteConfigClicked: root.model.deleteBus(index)
            onNameEdited: root.model.setBusName(index, name)
        }
    }

    Button {
        id: addMono
        text: "Add Mono"
        anchors.rightMargin: 8
        anchors.leftMargin: 8
        anchors.bottomMargin: 16
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        font.capitalization: Font.MixedCase

        onClicked: root.model.addMonoBus()
    }
}
