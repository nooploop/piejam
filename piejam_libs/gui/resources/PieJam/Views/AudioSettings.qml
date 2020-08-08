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
