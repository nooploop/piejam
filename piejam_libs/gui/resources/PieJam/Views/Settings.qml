// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../Controls"
import "../SettingsControls"

Pane {
    id: root

    property alias audioDeviceModel: audioSettings.deviceModel
    property alias audioInputModel: audioSettings.inputModel
    property alias audioOutputModel: audioSettings.outputModel
    property alias midiInputModel: midiSettings.model

    padding: 0

    RowLayout {
        anchors.fill: parent

        spacing: 0

        ListView {
            id: tabButtons

            Layout.preferredWidth: 96
            Layout.fillHeight: true

            spacing: 0
            interactive: false

            model: ["Audio", "MIDI"]

            delegate: Button {
                width: 96

                flat: true
                text: modelData
                highlighted: ListView.isCurrentItem

                onClicked: tabButtons.currentIndex = index
            }
        }

        Rectangle {
            Layout.preferredWidth: 4
            Layout.fillHeight: true

            gradient: Gradient {
                orientation: Gradient.Horizontal

                GradientStop {
                    position: 0
                    color: "#00000000"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: tabButtons.currentIndex

            AudioSettings {
                id: audioSettings
            }

            MidiSettings {
                id: midiSettings
            }
        }
    }
}
