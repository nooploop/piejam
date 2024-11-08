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

                onEditingFinished: {
                    root.model.changeName(nameText.text)
                    focus = false
                }
            }

            ColorSelector {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 52
                Layout.alignment: Qt.AlignHCenter

                selectedColor: root.model ? root.model.color : Material.Pink

                onColorSelected: root.model.changeColor(newSelectedColor)
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

            AudioRoutingComboBox {
                Layout.fillWidth: true

                defaultText: root.model ? (root.model.busType === PJModels.Types.BusType.Stereo ? "Mix" : "None") : "-"
                model: root.model ? root.model.in : null
            }

            Label {
                Layout.fillWidth: true

                textFormat: Text.PlainText
                text: qsTr("Audio Out")
            }

            AudioRoutingComboBox {
                Layout.fillWidth: true

                defaultText: "None"
                model: root.model ? root.model.out : null
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
