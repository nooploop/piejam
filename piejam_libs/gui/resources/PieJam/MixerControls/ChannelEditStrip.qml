// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import ".."

SubscribableItem {
    id: root

    property bool deletable: true

    implicitWidth: 132

    Frame {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: nameText

                Layout.fillWidth: true

                text: root.model ? root.model.name : ""

                placeholderText: qsTr("Name")

                onTextEdited: root.model.changeName(nameText.text)
            }

            RowLayout {
                id: moveButtonsRow

                Layout.fillWidth: true
                Layout.preferredHeight: 48
                Layout.maximumHeight: 48

                spacing: 8

                Button {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    enabled: root.model && root.model.canMoveLeft

                    text: qsTr("<")

                    onClicked: root.model.moveLeft()
                }

                Button {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    enabled: root.model && root.model.canMoveRight

                    text: qsTr(">")

                    onClicked: root.model.moveRight()
                }
            }

            Label {
                Layout.fillWidth: true

                textFormat: Text.PlainText
                text: qsTr("Audio In")
            }

            ComboBox {
                Layout.fillWidth: true

                displayText: root.model ? root.model.selectedInput : ""

                Material.foreground: root.model
                                            ? (PJModels.MixerChannelEdit.SelectedInputState.Invalid === root.model.selectedInputState
                                                ? Material.Red
                                                : Material.primaryTextColor)
                                            : Material.primaryTextColor

                popup: Menu {
                    MenuItem {
                        enabled: root.model ? root.model.defaultInputIsValid : true

                        text: root.model && (root.model.busType === PJModels.Types.BusType.Mono) ? qsTr("None") : qsTr("Mix")

                        onClicked: root.model.changeInputToDefault()
                    }

                    Menu {
                        title: qsTr("Devices")

                        enabled: inDevicesRep.count > 0

                        Repeater {
                            id: inDevicesRep

                            model: root.model ? root.model.inputDevices : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeInputToDevice(index)
                            }
                        }
                    }

                    Menu {
                        title: qsTr("Channels")

                        enabled: inChannelsRep.count > 0

                        Repeater {
                            id: inChannelsRep

                            model: root.model ? root.model.inputChannels : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeInputToChannel(index)
                            }
                        }
                    }
                }
            }

            Label {
                Layout.fillWidth: true

                textFormat: Text.PlainText
                text: qsTr("Audio Out")
            }

            ComboBox {
                Layout.fillWidth: true

                displayText: root.model ? root.model.selectedOutput : ""

                function selectedOutputStateToColor(s) {
                    switch (s) {
                        case PJModels.MixerChannelEdit.SelectedOutputState.Invalid:
                            return Material.Red

                        case PJModels.MixerChannelEdit.SelectedOutputState.NotMixed:
                            return Material.Yellow

                        default:
                            return Material.primaryTextColor
                    }
                }

                Material.foreground: selectedOutputStateToColor(root.model ? root.model.selectedOutputState : null)

                popup: Menu {

                    MenuItem {
                        text: qsTr("None")

                        onClicked: root.model.changeOutputToNone()
                    }

                    Menu {
                        title: qsTr("Devices")

                        enabled: outDevicesRep.count > 0

                        Repeater {
                            id: outDevicesRep

                            model: root.model ? root.model.outputDevices : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeOutputToDevice(index)
                            }
                        }
                    }

                    Menu {
                        title: qsTr("Channels")

                        enabled: outChannelsRep.count > 0

                        Repeater {
                            id: outChannelsRep

                            model: root.model ? root.model.outputChannels : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeOutputToChannel(index)
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Button {
                visible: root.deletable
                enabled: root.deletable

                Layout.fillWidth: true

                text: qsTr("Delete")

                Material.background: Material.color(Material.Red, Material.Shade400)

                onClicked: root.model.deleteChannel()
            }
        }
    }
}
