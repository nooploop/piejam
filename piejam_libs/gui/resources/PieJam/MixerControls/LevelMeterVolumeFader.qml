// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import "../Controls"

Item {
    id: root

    property alias levelLeft: meter.levelLeft
    property alias levelRight: meter.levelRight
    property alias volume: fader.volume
    property alias muted: meter.muted

    property alias volumeMidi: midiAssign.model

    property alias levelMeterScale: meter.scaleData
    property alias volumeFaderScale: fader.scaleData

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

    VolumeFader {
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
