// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import QtQml 2.15

import ".."
import "../Controls"

SubscribableItem {
    id: root

    implicitWidth: 800
    implicitHeight: 48

    Rectangle {
        id: statusBarBackground

        anchors.fill: parent

        color: Material.color(Material.Grey, Material.Shade800)

        Label {
            id: infoMessageLabel

            anchors.left: parent.left
            anchors.right: recordButton.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6

            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            clip: true

            padding: 2
            leftPadding: 4
            rightPadding: 4
            text: Info.message

            background: Rectangle {
                color: Material.background
                radius: 4
            }
        }

        Button {
            id: recordButton

            width: 48

            anchors.right: midiLearn.left
            anchors.rightMargin: 6

            checkable: true
            Material.accent: Material.Red

            checked: root.model.recording

            text: "REC"

            onClicked: root.model.changeRecording(checked)
        }

        Button {
            id: midiLearn

            width: 38

            anchors.right: infoReadout.left
            anchors.rightMargin: 6

            checkable: true

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/midi-port.svg"
            display: AbstractButton.IconOnly

            Binding {
                target: MidiLearn
                property: "active"
                value: midiLearn.checked
            }
        }

        InfoReadout {
            id: infoReadout

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6

            diskUsage: root.model.diskUsage
            audioLoad: root.model.audioLoad
            xruns: root.model.xruns
            cpuLoad: root.model.cpuLoad
            cpuTemp: root.model.cpuTemp
        }
    }

    Connections {
        target: root.model

        function onMidiLearnChanged() {
            if (!root.model.midiLearn)
                MidiLearn.stop()
        }
    }
}
