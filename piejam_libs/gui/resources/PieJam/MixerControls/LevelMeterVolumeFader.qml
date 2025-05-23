// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property alias peakLevel: meter.peakLevel
    property alias rmsLevel: meter.rmsLevel
    property alias volume: fader.model
    property bool muted: false

    property alias levelMeterScale: meter.scaleData
    property alias volumeFaderScale: fader.scaleData

    RowLayout {
        anchors.fill: parent
        spacing: 2

        StereoLevelMeter {
            id: meter

            Layout.fillHeight: true

            muted: root.muted
        }

        VolumeFader {
            id: fader

            Layout.fillWidth: true
            Layout.fillHeight: true

            muted: root.muted
        }
    }
}
