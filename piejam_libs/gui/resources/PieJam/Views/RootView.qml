// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.VirtualKeyboard 2.15
import QtQuick.VirtualKeyboard.Settings 2.15

Item {
    id: root

    property var modelFactory

    implicitWidth: 800
    implicitHeight: 480

    ToolBar {
        id: toolBar

        width: 48

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        ToolButton {
            id: btnMixer

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/tune-vertical.svg"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 2 }
        }

        ToolButton {
            id: btnSettings

            anchors.bottom: btnPower.top

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/cog.svg"
            display: AbstractButton.IconOnly

            onClicked: { content.currentIndex = 1 }
        }

        ToolButton {
            id: btnPower

            anchors.bottom: parent.bottom

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/power.svg"
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

            onFxButtonClicked: {
                fxChainPane.chainIndex = index
                content.currentIndex = 3
            }
        }

        FxChain {
            id: fxChainPane

            chainModel: root.modelFactory.mixer.fxChains
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

        y: parent.height - inputPanel.height

        visible: Qt.inputMethod.visible

        anchors.left: parent.left
        anchors.right: parent.right
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "fullScreenMode"
        value: Qt.inputMethod.visible
    }
}
