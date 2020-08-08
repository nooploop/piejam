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
import QtQuick.Layouts 1.12

import "../Controls"

Item {
    property alias name: title.text
    property alias levelLeft: levelMeterFader.levelLeft
    property alias levelRight: levelMeterFader.levelRight
    property alias pan: panControls.value
    property alias gain: levelMeterFader.gain

    signal faderMoved(real newGain)
    signal panMoved(real newPan)

    id: root

    width: 142
    height: 400

    Label {
        id: title

        height: 18
        anchors.rightMargin: 4
        anchors.leftMargin: 4
        anchors.top: parent.top
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

    BipolarSlider {
        id: panControls

        height: 40
        anchors.rightMargin: 6
        anchors.leftMargin: 6
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: title.bottom

        onMoved: root.panMoved(panControls.value)
    }

    LevelMeterFader {
        id: levelMeterFader

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top: panControls.bottom

        onFaderMoved: root.faderMoved(newGain)
    }

}
