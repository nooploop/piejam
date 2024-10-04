// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property int selectedColor: Material.Pink

    readonly property var colors: [
        Material.Pink,
        Material.DeepPurple,
        Material.Blue,
        Material.Cyan,
        Material.Green,
        Material.Lime,
        Material.Orange,
        Material.Red,
    ]

    signal colorSelected(var newSelectedColor)

    Button {
        anchors.fill: parent

        flat: true

        Material.background: Material.color(root.selectedColor)

        onClicked: popup.open()

        Popup {
            id: popup

            anchors.centerIn: Overlay.overlay

            GridLayout {
                columns: 4

                Repeater {
                    model: root.colors

                    delegate: Button {
                        flat: true

                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 52

                        Material.theme: Material.Light
                        Material.background: modelData

                        onClicked: {
                            root.colorSelected(root.colors[index])
                            popup.close()
                        }
                    }
                }
            }
        }
    }
}
