// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "../Controls"

Item {
    property bool mono: false
    property bool labelVisible: true
    property alias pan: panKnob.value
    property alias balance: balanceKnob.value

    signal panMoved()
    signal balanceMoved()

    id: root

    Row {
        id: row
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 8

        LabeledKnob {
            id: panKnob

            width: Math.min(root.width, root.height)
            height: panKnob.width

            bipolar: true
            text: (panKnob.value * 100).toFixed(0) + "%"
            labelVisible: root.labelVisible

            visible: root.mono

            onMoved: root.panMoved()
        }

        LabeledKnob {
            id: balanceKnob

            width: Math.min(root.width, root.height)
            height: balanceKnob.width

            bipolar: true
            text: (balanceKnob.value * 100).toFixed(0) + "%"
            labelVisible: root.labelVisible

            visible: !root.mono

            onMoved: root.balanceMoved()
        }
    }
}
