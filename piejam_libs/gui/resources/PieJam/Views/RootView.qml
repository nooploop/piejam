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

    property var modelFactory: null

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
                        if (root.modelFactory.rootView.mode === 0)
                            MixerViewSettings.switchMode(MixerViewSettings.perform)
                        else
                            root.modelFactory.rootView.showMixer()
                    }
                }

                ToolButton {
                    text: "FX"

                    enabled: root.modelFactory.rootView.canShowFxModule

                    onClicked: root.modelFactory.rootView.showFxModule()
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                InfoToolButton {
                    property int lastMessagesCount: 0
                    property var logMessages: root.modelFactory.log.logMessages

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
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/cog.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.modelFactory.rootView.showSettings()
                }

                ToolButton {
                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/power.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.modelFactory.rootView.showPower()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            spacing: 0

            StatusBar {
                Layout.fillWidth: true

                model: root.modelFactory.info
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 2

                color: "#000000"
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: root.modelFactory.rootView.mode

                Mixer {
                    model: root.modelFactory.mixer
                }

                Loader {
                    sourceComponent: Log {
                        logMessages: root.modelFactory.log.logMessages
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: Settings {
                        audioDeviceModel: root.modelFactory.audioDeviceSettings
                        audioInputModel: root.modelFactory.audioInputSettings
                        audioOutputModel: root.modelFactory.audioOutputSettings
                        midiInputModel: root.modelFactory.midiInputSettings
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
                        model: root.modelFactory.fxBrowser
                    }
                    asynchronous: true
                }

                Loader {
                    sourceComponent: FxModule {
                        model: root.modelFactory.fxModule
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
        target: root.modelFactory.rootView
        subscribed: root.visible
    }
}
