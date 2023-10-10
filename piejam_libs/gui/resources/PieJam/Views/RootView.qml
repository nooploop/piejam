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

    property var modelManager: null

    implicitWidth: 800
    implicitHeight: 480

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
                        if (root.modelManager.rootView.mode === 0)
                            MixerViewSettings.switchMode(MixerViewSettings.perform)
                        else
                            root.modelManager.rootView.showMixer()
                    }
                }

                ToolButton {
                    text: "FX"

                    enabled: root.modelManager.rootView.canShowFxModule

                    onClicked: root.modelManager.rootView.showFxModule()
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
                        logMessages: root.modelManager.log.logMessages
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
