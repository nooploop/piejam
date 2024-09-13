// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import ".."

SubscribableItem {
    id: root

    property bool showAddMono: true
    property bool showAddStereo: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8

        spacing: 8

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: root.model.deviceConfigs

            spacing: 8

            clip: true

            delegate: ExternalAudioDeviceConfig {
                height: 64
                name: model.item.name
                mono: model.item.mono
                channels: root.model.channels
                monoChannelIndex: model.item.monoChannel
                stereoLeftChannelIndex: model.item.stereoLeftChannel
                stereoRightChannelIndex: model.item.stereoRightChannel

                anchors.left: parent ? parent.left : undefined
                anchors.right: parent ? parent.right : undefined

                onMonoChannelSelected: model.item.changeMonoChannel(ch)
                onStereoLeftChannelSelected: model.item.changeStereoLeftChannel(ch)
                onStereoRightChannelSelected: model.item.changeStereoRightChannel(ch)
                onDeleteConfigClicked: model.item.remove()
                onNameEdited: model.item.changeName(name)

                ModelSubscription {
                    target: model.item
                    subscribed: visible
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            spacing: 8

            Button {
                Layout.fillWidth: true

                text: "+Mono"
                visible: root.showAddMono

                onClicked: root.model.addMonoDevice()
            }

            Button {
                Layout.fillWidth: true

                text: "+Stereo"
                visible: root.showAddStereo

                onClicked: root.model.addStereoDevice()
            }
        }
    }
}
