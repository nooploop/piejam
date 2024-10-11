// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Items 1.0 as PJItems

import "../Util"

Item {
    PJItems.FixedLogScaleGrid {
        anchors.fill: parent

        ticks: [
            10.0,   20.0,   30.0,   40.0,    50.0,   60.0,
            70.0,   80.0,   90.0,   100.0,   200.0,  300.0,
            400.0,  500.0,  600.0,  700.0,   800.0,  900.0,
            1000.0, 2000.0, 3000.0, 4000.0,  5000.0, 6000.0,
            7000.0, 8000.0, 9000.0, 10000.0, 20000.0
        ]

        Repeater {
            model: [10, 100, 1000, 10000]

            delegate: Label {
                x: MathExt.mapTo(Math.log10(modelData), 1, Math.log10(20000), 0, parent.width) + 2
                y: parent.height - height - 2

                font.pointSize: 6

                text: modelData >= 1000 ? (modelData / 1000) + " kHz" : modelData + " Hz"
            }
        }
    }

    PJItems.FixedLinearScaleGrid {
        anchors.fill: parent

        orientation: Qt.Vertical
        ticks: [0, -20, -40, -60, -80, -100]

        Repeater {
            model: [0, -20, -40, -60, -80]

            delegate: Label {
                x: 2
                y: MathExt.mapTo(modelData, 0, -100, 0, parent.height) + 2

                font.pointSize: 6

                text: modelData + " dB"
            }
        }
    }
}
