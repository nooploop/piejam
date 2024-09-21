// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "../Controls"
import "../SettingsControls"

Item {

    property alias deviceModel: deviceSettings.model
    property alias inputModel: audioInputSettings.model
    property alias outputModel: audioOutputSettings.model

    id: root

    ColumnLayout {
        anchors.fill: parent

        TabBar {
            id: tabs

            Layout.fillWidth: true

            currentIndex: 0

            TabButton {
                text: "Device"
            }

            TabButton {
                text: "Input"
            }

            TabButton {
                text: "Output"
            }
        }

        StackLayout {
            id: stackView

            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: tabs.currentIndex

            AudioDeviceSettings {
                id: deviceSettings
            }

            AudioInputOutputSettings {
                id: audioInputSettings

                showAddMono: true
                showAddStereo: true
            }

            AudioInputOutputSettings {
                id: audioOutputSettings

                showAddMono: false
                showAddStereo: true
            }
        }
    }
}
