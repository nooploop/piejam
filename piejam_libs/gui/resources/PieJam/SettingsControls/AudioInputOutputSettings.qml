// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13

Item {
    id: root

    property var model
    property bool showAddMono: true
    property bool showAddStereo: true

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

            onMonoChannelSelected: model.item.changeMonoChannel(ch)
            onStereoLeftChannelSelected: model.item.changeStereoLeftChannel(ch)
            onStereoRightChannelSelected: model.item.changeStereoRightChannel(ch)
            onDeleteConfigClicked: model.item.deleteBus()
            onNameEdited: model.item.changeName(name)

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
