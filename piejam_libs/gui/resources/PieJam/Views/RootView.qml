// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.VirtualKeyboard 2.15
import QtQuick.VirtualKeyboard.Settings 2.15

import QtQml 2.15

import PieJam.Controls 1.0
import PieJam.MixerControls 1.0
import PieJam.Models 1.0 as PJModels
import PieJam.Util 1.0

SubscribableItem {
    id: root

    property PJModels.Root model: null

    RowLayout {
        anchors.fill: parent

        spacing: 0

        ToolBar {
            Layout.preferredWidth: 48
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                spacing: 0

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/tune-vertical.svg"
                    display: AbstractButton.IconOnly

                    onClicked: {
                        MixerViewSettings.forceMode(MixerViewSettings.perform)
                        root.model.showMixer()
                    }
                }

                ToolButton {
                    text: "FX"
                    font.bold: true

                    onClicked: {
                        if (root.model.canShowFxModule && root.model.mode !== PJModels.Root.Mode.FxModule) {
                            root.model.showFxModule()
                        } else {
                            MixerViewSettings.forceMode(MixerViewSettings.fx)
                            root.model.showMixer()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                InfoToolButton {
                    property int lastMessagesCount: 0
                    property var logMessages: root.model.log.logMessages

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/info.svg"
                    display: AbstractButton.IconOnly

                    onClicked: {
                        lastMessagesCount = logMessages.count
                        root.model.showInfo()
                    }

                    info: logMessages.count === lastMessagesCount ? "" : logMessages.count - lastMessagesCount
                }

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/cog.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.model.showSettings()
                }

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/power.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.model.showPower()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 0

            StatusBar {
                Layout.fillWidth: true

                model: root.model.info
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2

                color: "#000000"
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: root.model.mode

                Mixer {
                    model: root.model.mixer
                }

                Loader {
                    sourceComponent: Log {
                        model: root.model.log
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: Settings {
                        audioDeviceModel: root.model.audioDeviceSettings
                        audioInputModel: root.model.audioInputSettings
                        audioOutputModel: root.model.audioOutputSettings
                        midiInputModel: root.model.midiInputSettings
                        uiSettingsModel: root.model.uiSettings
                        sessionModel: root.model.sessionSettings
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: Power {
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: FxBrowser {
                        model: root.model.fxBrowser
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: FxModule {
                        model: root.model.fxModule
                    }
                    asynchronous: true
                }
            }
        }
    }

    InputPanel {
        id: inputPanel

        y: parent.height - inputPanel.height

        visible: root.model.onScreenKeyboardEnabled && Qt.inputMethod.visible

        anchors.left: parent.left
        anchors.right: parent.right
    }

    Connections {
        target: inputPanel.keyboard

        function onLayoutChanged() {
            if (inputPanel.keyboard.layout !== "") {
                var hideKey = QmlExt.findChild(inputPanel.keyboard, function(obj) { return obj instanceof HideKeyboardKey })
                if (hideKey !== null) {
                    hideKey.visible = false
                }

                var changeLanguageKey = QmlExt.findChild(inputPanel.keyboard, function(obj) { return obj instanceof ChangeLanguageKey })
                if (changeLanguageKey !== null) {
                    changeLanguageKey.visible = false
                }
            }
        }
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "fullScreenMode"
        value: root.model.onScreenKeyboardEnabled && Qt.inputMethod.visible
    }
}
