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

Item {
    property alias bipolar: knob.bipolar
    property alias text: label.text
    readonly property alias value: knob.value
    property alias labelVisible: label.visible

    id: root

    Knob {
        id: knob

        anchors.bottomMargin: 2
        anchors.bottom: root.labelVisible ? label.top : parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
    }

    Label {
        id: label

        text: knob.value.toFixed(2)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
