// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../Controls"

Item {
    id: root

    property alias levelLeft: meter.levelLeft
    property alias levelRight: meter.levelRight
    property alias volume: fader.volume
    property alias muted: meter.muted

    signal faderMoved(real newVolume)

    width: 142
    height: 400

    LevelMeter {
        id: meter

        anchors.right: fader.left
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    LevelFader {
        id: fader

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        onMoved: root.faderMoved(newVolume)

        MidiAssignArea {
            id: midiAssign

            anchors.fill: parent
        }
    }
}
