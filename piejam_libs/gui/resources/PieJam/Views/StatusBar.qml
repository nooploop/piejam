// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import ".."
import "../Controls"

SubscribableItem {
    id: root

    implicitHeight: 48

    Rectangle {
        anchors.fill: parent

        color: Material.color(Material.Grey, Material.Shade800)

        RowLayout {
            anchors.fill: parent

            Label {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: 6
                Layout.bottomMargin: 6
                Layout.leftMargin: 6

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
                Layout.preferredWidth: 48

                checkable: true
                Material.accent: Material.Red

                checked: root.model.recording

                text: "REC"

                onClicked: root.model.changeRecording(checked)
            }

            Button {
                id: midiLearn

                Layout.preferredWidth: 38

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
                Layout.fillHeight: true
                Layout.topMargin: 6
                Layout.bottomMargin: 6
                Layout.rightMargin: 6

                diskUsage: root.model.diskUsage
                audioLoad: root.model.audioLoad
                xruns: root.model.xruns
                cpuLoad: root.model.cpuLoad
                cpuTemp: root.model.cpuTemp
            }
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
