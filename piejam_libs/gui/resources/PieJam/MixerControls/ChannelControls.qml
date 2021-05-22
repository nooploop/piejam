// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.12

import "../Controls"

Item {
    id: root

    property alias solo: soloButton.checked
    property alias mute: muteButton.checked

    property alias muteMidi: midiAssignMute
    property alias soloMidi: midiAssignSolo

    signal soloToggled()
    signal muteToggled()

    Row {
        anchors.fill: parent

        spacing: 6

        Button {
            id: recordButton

            width: 32
            height: 40

            text: qsTr("R")
            checkable: true
            Material.accent: Material.Red
        }

        Button {
            id: soloButton

            width: 32
            height: 40

            text: qsTr("S")
            checkable: true
            Material.accent: Material.Yellow

            onToggled: root.soloToggled()

            MidiAssignArea {
                id: midiAssignSolo

                anchors.fill: parent
                anchors.topMargin: 1
                anchors.bottomMargin: 1
            }
        }

        Button {
            id: muteButton

            width: 32
            height: 40

            text: qsTr("M")
            checkable: true

            onToggled: root.muteToggled()

            MidiAssignArea {
                id: midiAssignMute

                anchors.fill: parent
                anchors.topMargin: 1
                anchors.bottomMargin: 1
            }
        }
    }
}
