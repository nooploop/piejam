// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property var model

    TabBar {
        id: tabs

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        currentIndex: 0

        TabButton {
            text: "Input"
        }
    }

    StackLayout {
        id: stackView

        anchors.top: tabs.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        currentIndex: tabs.currentIndex

        ListView {
            id: midiDevicesList

            orientation: ListView.Vertical

            model: root.model.devices

            delegate: MidiDeviceConfig {
                name: model.item.name
                deviceEnabled: model.item.enabled

                anchors.left: if (parent) parent.left
                anchors.right: if (parent) parent.right

                onEnabledToggled: model.item.changeEnabled(newEnabled)

                Binding {
                    target: model.item
                    property: "subscribed"
                    value: visible
                }
            }
        }
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
