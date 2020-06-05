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

Frame {
    id: root

    property alias level: meter.level
    property alias name: title.text
    property alias gain: fader.gain

    signal faderMoved(real newGain)

    width: 142
    height: 400
    padding: 8

    LevelMeter {
        id: meter
        anchors.topMargin: 8
        anchors.rightMargin: 4
        anchors.right: fader.left
        anchors.left: parent.left
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
    }

    LevelFader {
        id: fader
        anchors.topMargin: 8
        anchors.top: title.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        onMoved: root.faderMoved(newGain)
    }

    Label {
        id: title
        y: 0
        height: 18
        padding: 2
        verticalAlignment: Text.AlignVCenter
        background: Rectangle {
            color: Material.primaryColor
            radius: 2
        }
        font.bold: true
        anchors.right: parent.right
        anchors.left: parent.left
        font.pixelSize: 12
    }
}
