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

    property string defaultText: "-"

    implicitHeight: comboBox.implicitHeight

    ComboBoxElided {
        id: comboBox

        anchors.fill: parent

        displayTextToElide: root.model
                            ? (root.model.selected.isDefault
                                ? (root.model.selected.state === PJModels.AudioRoutingSelection.State.Valid ? root.defaultText : "???")
                                : root.model.selected.label)
                            : "-"
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
            MenuItem {
                enabled: root.model && root.model.defaultIsValid

                text: root.defaultText

                onClicked: root.model.changeToDefault()
            }

            Menu {
                title: qsTr("Devices")

                enabled: devicesRep.count > 0

                Repeater {
                    id: devicesRep

                    model: root.model ? root.model.devices : null

                    delegate: MenuItem {
                        text: model.item.value

                        onClicked: root.model.changeToDevice(index)

                        ModelSubscription {
                            target: model.item
                            subscribed: parent.visible
                        }
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
}
