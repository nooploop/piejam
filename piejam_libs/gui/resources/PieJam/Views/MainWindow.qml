// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13
import QtQuick.VirtualKeyboard 2.13
import QtQuick.VirtualKeyboard.Settings 2.13
import QtQuick.Window 2

Window {
    id: root

    property var modelFactory

    width: 800
    height: 480
    color: "#00000000"
    visible: true

    Material.theme: Material.Dark
    Material.primary: Material.Pink
    Material.accent: Material.Pink

    ToolBar {
        id: toolBar

        width: 48

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        ToolButton {
            id: btnMixer

            icon.source: "qrc:///images/icons/mixer.png"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 2 }
        }

        ToolButton {
            id: btnSettings

            anchors.bottom: btnPower.top

            icon.source: "qrc:///images/icons/settings.png"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 1 }
        }

        ToolButton {
            id: btnPower

            anchors.bottom: parent.bottom

            icon.source: "qrc:///images/icons/power.png"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 0 }
        }
    }

    StackLayout {
        id: content

        anchors.left: toolBar.right
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.bottom: parent.bottom

        currentIndex: 1

        Power {
            id: powerPane
        }

        Settings {
            id: audioSettingsPane

            audioDeviceModel: root.modelFactory.audioDeviceSettings
            audioInputModel: root.modelFactory.audioInputSettings
            audioOutputModel: root.modelFactory.audioOutputSettings
            midiInputModel: root.modelFactory.midiInputSettings
        }

        Mixer {
            id: mixerPane

            model: root.modelFactory.mixer

            onFxButtonClicked: content.currentIndex = 3
        }

        FxChain {
            id: fxChainPane

            model: root.modelFactory.fxChain
            browser: root.modelFactory.fxBrowser
        }
    }

    StatusBar {
        id: statusBar

        anchors.left: toolBar.right
        anchors.right: parent.right
        anchors.top: parent.top

        model: root.modelFactory.info

        Rectangle {
            id: statusBarSeparator

            height: 2
            color: "#000000"

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
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
