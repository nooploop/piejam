// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13
import QtQuick.VirtualKeyboard 2.13
import QtQuick.VirtualKeyboard.Settings 2.13

ApplicationWindow {
    id: root

    property var modelFactory

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

        Settings {
            id: audioSettingsPane

            audioDeviceModel: root.modelFactory.audioDeviceSettings
            audioInputModel: root.modelFactory.audioInputSettings
            audioOutputModel: root.modelFactory.audioOutputSettings
        }

        Mixer {
            id: mixerPane

            model: root.modelFactory.mixer

            onFxButtonClicked: content.currentIndex = 4
        }

        Info {
            id: infoPane

            model: root.modelFactory.info
        }

        FxChain {
            id: fxChainPane

            model: root.modelFactory.fxChain
            browser: root.modelFactory.fxBrowser
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
