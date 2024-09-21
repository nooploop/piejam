// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"

SubscribableItem {
    id: root

    implicitWidth: 132

    Frame {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            HeaderLabel {
                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? root.model.name : ""
            }

            ComboBoxElided {
                id: comboBox

                Layout.fillWidth: true

                displayTextToElide: root.model ? root.model.selected.label : "-"
                elideMode: Qt.ElideMiddle

                function selectedStateToColor(s) {
                    switch (s) {
                        case PJModels.AudioRoutingSelection.State.Invalid:
                            return Material.Red

                        case PJModels.AudioRoutingSelection.State.NotMixed:
                            return Material.Yellow

                        default:
                            return Material.primaryTextColor
                    }
                }

                Material.foreground: selectedStateToColor(root.model ? root.model.selected.state : null)

                popup: Menu {
                    Menu {
                        title: qsTr("Devices")

                        enabled: devicesRep.count > 0

                        Repeater {
                            id: devicesRep

                            model: root.model ? root.model.devices : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeToDevice(index)
                            }
                        }
                    }

                    Menu {
                        title: qsTr("Channels")

                        enabled: channelsRep.count > 0

                        Repeater {
                            id: channelsRep

                            model: root.model ? root.model.channels : []

                            delegate: MenuItem {
                                text: modelData

                                onClicked: root.model.changeToChannel(index)
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent

                    VolumeFader {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        visible: root.model && root.model.volume

                        model: root.model ? root.model.volume : null

                        scaleData: PJModels.MixerDbScales.sendFaderScale

                        onMoved: {
                            if (root.model && root.model.canToggle && !root.model.enabled)
                            {
                                root.model.toggleEnabled()
                            }
                        }
                    }

                    Button {
                        Layout.fillWidth: true

                        visible: root.model && root.model.volume

                        icon.source: (!root.model || root.model.canToggle)
                                     ? "qrc:///images/icons/power.svg"
                                     : "qrc:///images/icons/cycle_arrows.svg"
                        checkable: true
                        checked: root.model && root.model.enabled
                        enabled: root.model && root.model.canToggle

                        onClicked: if (root.model) root.model.toggleEnabled()
                    }
                }
            }
        }
    }
}
