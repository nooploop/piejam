// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.VirtualKeyboard 2.15
import QtQuick.VirtualKeyboard.Settings 2.15

import QtQml 2.15

import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"
import "../MixerControls"
import "../Util/QmlExt.js" as QmlExt

Item {
    id: root

    property var modelManager: null

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
                        root.modelManager.rootView.showMixer()
                    }
                }

                ToolButton {
                    text: "FX"

                    onClicked: {
                        if (root.modelManager.rootView.canShowFxModule && root.modelManager.rootView.mode !== PJModels.RootView.Mode.FxModule) {
                            root.modelManager.rootView.showFxModule()
                        } else {
                            MixerViewSettings.forceMode(MixerViewSettings.fx)
                            root.modelManager.rootView.showMixer()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                InfoToolButton {
                    property int lastMessagesCount: 0
                    property var logMessages: root.modelManager.log.logMessages

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/info.svg"
                    display: AbstractButton.IconOnly

                    onClicked: {
                        lastMessagesCount = logMessages.length
                        root.modelManager.rootView.showInfo()
                    }

                    info: logMessages.length === lastMessagesCount ? "" : logMessages.length - lastMessagesCount
                }

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/cog.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.modelManager.rootView.showSettings()
                }

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/power.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.modelManager.rootView.showPower()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 0

            StatusBar {
                Layout.fillWidth: true

                model: root.modelManager.info
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2

                color: "#000000"
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: root.modelManager.rootView.mode

                Mixer {
                    model: root.modelManager.mixer
                }

                Loader {
                    sourceComponent: Log {
                        model: root.modelManager.log
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: Settings {
                        audioDeviceModel: root.modelManager.audioDeviceSettings
                        audioInputModel: root.modelManager.audioInputSettings
                        audioOutputModel: root.modelManager.audioOutputSettings
                        midiInputModel: root.modelManager.midiInputSettings
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
                        model: root.modelManager.fxBrowser
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: FxModule {
                        model: root.modelManager.fxModule
                    }
                    asynchronous: true
                }
            }
        }
    }

    InputPanel {
        id: inputPanel

        y: parent.height - inputPanel.height

        visible: Qt.inputMethod.visible

        anchors.left: parent.left
        anchors.right: parent.right
    }

    Connections {
        target: inputPanel.keyboard

        function onLayoutChanged() {
            if (inputPanel.keyboard.layout !== "") {
                var hideKey = QmlExt.findChild(inputPanel.keyboard, function(obj) { return obj instanceof HideKeyboardKey })
                if (hideKey !== null)
                    hideKey.visible = false

                var changeLanguageKey = QmlExt.findChild(inputPanel.keyboard, function(obj) { return obj instanceof ChangeLanguageKey })
                if (changeLanguageKey !== null)
                    changeLanguageKey.visible = false
            }
        }
    }

    Binding {
        target: VirtualKeyboardSettings
        property: "fullScreenMode"
        value: Qt.inputMethod.visible
    }

    ModelSubscription {
        target: root.modelManager.rootView
        subscribed: root.visible
    }
}
