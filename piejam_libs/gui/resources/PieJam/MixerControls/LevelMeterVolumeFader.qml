// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property var level: null
    property alias volume: fader.model
    property alias muted: meter.muted

    property alias levelMeterScale: meter.scaleData
    property alias volumeFaderScale: fader.scaleData

    RowLayout {
        anchors.fill: parent
        spacing: 0

        StereoLevelMeter {
            id: meter

            levelLeft: root.level ? root.level.levelLeft : 0
            levelRight: root.level ? root.level.levelRight : 0

            Layout.fillHeight: true
        }

        VolumeFader {
            id: fader

            Layout.fillHeight: true
        }
    }
}
