// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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

            delegate: Item {
                property var itemModel: model.item

                height: 64

                anchors.left: parent ? parent.left : undefined
                anchors.right: parent ? parent.right : undefined

                ExternalAudioDeviceConfig {
                    anchors.fill: parent

                    model: itemModel
                    channels: root.model ? root.model.channels : ["-"]
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
