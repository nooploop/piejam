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
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13
import QtQuick.VirtualKeyboard 2.13
import QtQuick.VirtualKeyboard.Settings 2.13

ApplicationWindow {
    id: root

    property alias audioSettingsPage: audioSettingsPane
    property alias mixerPage: mixerPane
    property alias infoPage: infoPane
    property alias fxChainPage: fxChainPane

    width: 800
    height: 480
    color: "#000000"
    visible: true

    Material.theme: Material.Dark
    Material.primary: Material.Pink
    Material.accent: Material.Pink

    header: ToolBar {
        width: 800
        height: 48

        ToolButton {
            id: btnMixer

            icon.source: "qrc:///images/icons/mixer.png"
            display: AbstractButton.IconOnly
            anchors.left: parent.left

            onClicked: { content.currentIndex = 2 }
        }

        ToolButton {
            id: btnInfo

            anchors.right: btnAudioSettings.left
            icon.source: "qrc:///images/icons/ic_info_outline_white_18dp.png"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 3 }
        }

        ToolButton {
            id: btnAudioSettings

            icon.source: "qrc:///images/icons/settings.png"
            display: AbstractButton.IconOnly
            anchors.right: btnPower.left

            onClicked: { content.currentIndex = 1 }
        }

        ToolButton {
            id: btnPower

            icon.source: "qrc:///images/icons/power.png"
            display: AbstractButton.IconOnly
            anchors.right: parent.right

            onClicked: { content.currentIndex = 0 }
        }
    }

    StackLayout {
        id: content

        currentIndex: 1

        Power {
            id: powerPane
        }

        AudioSettings {
            id: audioSettingsPane
        }

        Mixer {
            id: mixerPane

            onFxButtonClicked: content.currentIndex = 4
        }

        Info {
            id: infoPane
        }

        FxChain {
            id: fxChainPane
        }
    }

    InputPanel {
        id: inputPanel
        y: Qt.inputMethod.visible ? parent.height - inputPanel.height : parent.height
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "fullScreenMode"
        value: Qt.inputMethod.visible
    }
}
