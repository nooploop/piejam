// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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

    TabBar {
        id: tabs

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

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

        anchors.top: tabs.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left

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
