// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property var peakLevel: null
    property var rmsLevel: null
    property alias volume: fader.model
    property alias muted: meter.muted

    property alias levelMeterScale: meter.scaleData
    property alias volumeFaderScale: fader.scaleData

    RowLayout {
        anchors.fill: parent
        spacing: 0

        StereoLevelMeter {
            id: meter

            peakLevel: root.peakLevel
            rmsLevel: root.rmsLevel

            Layout.fillHeight: true
        }

        VolumeFader {
            id: fader

            Layout.fillHeight: true
        }
    }
}
