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
    property alias labelVisible: leftPan.labelVisible

    id: root

    Row {
        id: row
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 8

        LabeledKnob {
            id: leftPan

            width: Math.min(root.width, root.height)
            height: leftPan.width

            bipolar: true
            text: (leftPan.value * 100).toFixed(0) + "%"
        }

        LabeledKnob {
            id: rightPan

            width: leftPan.width
            height: leftPan.height

            bipolar: true
            text: (rightPan.value * 100).toFixed(0) + "%"
            labelVisible: root.labelVisible

            visible: !root.mono
        }
    }
}
