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

    Material.primary: root.model ? root.model.color : Material.Pink
    Material.accent: root.model ? root.model.color : Material.Pink

    Frame {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            HeaderLabel {
                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? root.model.name : ""
            }

            AudioRoutingComboBox {
                Layout.fillWidth: true

                allowDefaultSelection: false
                defaultText: "Send"
                model: root.model ? root.model.aux : null
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
