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
import QtQuick.Layouts 1.13

Item {

    property variant model

    property bool showAddMono: true
    property bool showAddStereo: true

    id: root

    ListView {
        id: busConfigsList

        model: root.model.busConfigs

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: addButtons.top
        anchors.margins: 8

        spacing: 8

        clip: true

        delegate: BusConfig {
            height: 64
            name: model.item.name
            mono: model.item.mono
            channels: root.model.channels
            monoChannelIndex: model.item.monoChannel
            stereoLeftChannelIndex: model.item.stereoLeftChannel
            stereoRightChannelIndex: model.item.stereoRightChannel

            anchors.left: if (parent) parent.left
            anchors.right: if (parent) parent.right

            onMonoChannelSelected: root.model.selectMonoChannel(index, ch)
            onStereoLeftChannelSelected: root.model.selectStereoLeftChannel(index, ch)
            onStereoRightChannelSelected: root.model.selectStereoRightChannel(index, ch)
            onDeleteConfigClicked: root.model.deleteBus(index)
            onNameEdited: root.model.setBusName(index, name)

            Binding {
                target: model.item
                property: "subscribed"
                value: visible
            }
        }
    }

    RowLayout {
        id: addButtons

        spacing: 8
        anchors.rightMargin: 8
        anchors.leftMargin: 8
        anchors.bottomMargin: 8

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left

        Button {
            id: addMono

            text: "Add Mono"
            Layout.fillWidth: true
            Layout.minimumWidth: addButtons.width / 2
            font.capitalization: Font.MixedCase
            visible: root.showAddMono

            onClicked: root.model.addMonoBus()
        }

        Button {
            id: addStereo

            text: "Add Stereo"
            Layout.fillWidth: true
            Layout.minimumWidth: addButtons.width / 2
            font.capitalization: Font.MixedCase
            visible: root.showAddStereo

            onClicked: root.model.addStereoBus()
        }

    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
