// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Models 1.0 as PJModels

Item {
    id: root

    property string name: "X"
    property bool active: true
    property int channel: PJModels.StereoChannel.Left

    signal activeToggled()
    signal channelSelected(int ch)

    implicitWidth: buttons.width
    implicitHeight: buttons.height

    Row {
        id: buttons

        readonly property int buttonWidth: 32
        readonly property int buttonHeight: 40

        spacing: 4

        Button {
            id: activateButton

            width: buttons.buttonWidth
            height: buttons.buttonHeight
            highlighted: root.active

            text: root.name
            flat: true

            onClicked: root.activeToggled()
        }

        Button {
            id: leftButton

            width: buttons.buttonWidth
            height: buttons.buttonHeight

            Material.accent: if (!root.active) Material.Grey

            text: qsTr("L")
            autoExclusive: true
            checkable: true
            checked: root.channel === PJModels.StereoChannel.Left

            onClicked: root.channelSelected(PJModels.StereoChannel.Left)
        }

        Button {
            id: rightButton

            width: buttons.buttonWidth
            height: buttons.buttonHeight

            Material.accent: if (!root.active) Material.Grey

            text: qsTr("R")
            autoExclusive: true
            checkable: true
            checked: root.channel === PJModels.StereoChannel.Right

            onClicked: root.channelSelected(PJModels.StereoChannel.Right)
        }

        Button {
            id: middleButton

            width: buttons.buttonWidth
            height: buttons.buttonHeight

            Material.accent: if (!root.active) Material.Grey

            text: qsTr("M")
            autoExclusive: true
            checkable: true
            checked: root.channel === PJModels.StereoChannel.Middle

            onClicked: root.channelSelected(PJModels.StereoChannel.Middle)
        }

        Button {
            id: sideButton

            width: buttons.buttonWidth
            height: buttons.buttonHeight

            Material.accent: if (!root.active) Material.Grey

            text: qsTr("S")
            autoExclusive: true
            checkable: true
            checked: root.channel === PJModels.StereoChannel.Side

            onClicked: root.channelSelected(PJModels.StereoChannel.Side)
        }
    }
}
