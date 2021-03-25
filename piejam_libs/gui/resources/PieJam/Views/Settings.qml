// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../Controls"
import "../SettingsControls"

ViewPane {

    id: root

    property alias audioDeviceModel: audioSettings.deviceModel
    property alias audioInputModel: audioSettings.inputModel
    property alias audioOutputModel: audioSettings.outputModel
    property alias midiInputModel: midiSettings.model

    property int currentIndex: 0

    ColumnLayout {
        id: tabBar

        width: 100

        TabButton {
            id: audioTab
            text: "Audio"
            Layout.fillWidth: true

            checked: root.currentIndex == 0

            onClicked: root.currentIndex = 0
        }

        TabButton {
            id: midiTab
            text: "MIDI"
            Layout.fillWidth: true

            checked: root.currentIndex == 1

            onClicked: root.currentIndex = 1
        }
    }

    Rectangle {
        id: tabsShadow

        width: 4

        anchors.right: settings.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

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
        id: settings

        anchors.left: tabBar.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        currentIndex: root.currentIndex

        AudioSettings {
            id: audioSettings
        }

        MidiSettings {
            id: midiSettings
        }
    }
}
