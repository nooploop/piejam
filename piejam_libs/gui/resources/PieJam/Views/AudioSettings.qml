// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13

import "../Controls"
import "../SettingsControls"

TopPane {

    property alias deviceModel: deviceSettings.model
    property alias inputModel: audioInputSettings.model
    property alias outputModel: audioOutputSettings.model

    id: root

    TabBar {
        id: tabs
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        currentIndex: swipeView.currentIndex

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

    SwipeView {
        id: swipeView
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
