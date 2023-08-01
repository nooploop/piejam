// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.VirtualKeyboard 2.15
import QtQuick.VirtualKeyboard.Settings 2.15

import QtQml 2.15

import ".."
import "../Controls"
import "../MixerControls"

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

            onClicked: {
                if (root.modelFactory.rootView.mode === 0)
                    MixerViewSettings.switchMode(MixerViewSettings.perform)
                else
                    root.modelFactory.rootView.showMixer()
            }
        }

        InfoToolButton {
            id: btnLog

            property int lastMessagesCount: 0
            property var logMessages: root.modelFactory.log.logMessages

            anchors.bottom: btnSettings.top

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/info.svg"
            display: AbstractButton.IconOnly

            onClicked: {
                lastMessagesCount = logMessages.length
                root.modelFactory.rootView.showInfo()
            }

            info: logMessages.length === lastMessagesCount ? "" : logMessages.length - lastMessagesCount
        }

        ToolButton {
            id: btnSettings

            anchors.bottom: btnPower.top

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/cog.svg"
            display: AbstractButton.IconOnly

            onClicked: root.modelFactory.rootView.showSettings()
        }

        ToolButton {
            id: btnPower

            anchors.bottom: parent.bottom

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/power.svg"
            display: AbstractButton.IconOnly

            onClicked: root.modelFactory.rootView.showPower()
        }
    }

    StackLayout {
        id: content

        anchors.left: toolBar.right
        anchors.right: parent.right
        anchors.top: statusBar.bottom
        anchors.bottom: parent.bottom

        currentIndex: root.modelFactory.rootView.mode

        Mixer {
            id: mixerPane

            model: root.modelFactory.mixer
        }

        Log {
            id: logPane

            logMessages: root.modelFactory.log.logMessages
        }

        Settings {
            id: audioSettingsPane

            audioDeviceModel: root.modelFactory.audioDeviceSettings
            audioInputModel: root.modelFactory.audioInputSettings
            audioOutputModel: root.modelFactory.audioOutputSettings
            midiInputModel: root.modelFactory.midiInputSettings
        }

        Power {
            id: powerPane
        }

        FxBrowser {
            id: fxBrowserPane

            model: root.modelFactory.fxBrowser
        }

        FxModule {
            id: fxModulePane

            model: root.modelFactory.fxModule
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

    ModelSubscription {
        target: root.modelFactory.rootView
        subscribed: root.visible
    }
}
