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

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

import "../Controls"

Item {
    property alias name: title.text
    property alias levelLeft: levelMeterFader.levelLeft
    property alias levelRight: levelMeterFader.levelRight
    property alias meterMuted: levelMeterFader.muted
    property alias pan: panControls.value
    property alias volume: levelMeterFader.volume
    property alias mute: channelControls.mute
    property alias solo: channelControls.solo
    property alias soloEnabled: channelControls.soloEnabled

    signal faderMoved(real newVolume)
    signal panMoved(real newPan)
    signal muteToggled()
    signal soloToggled()
    signal fxButtonClicked()

    id: root

    width: 150
    height: 400

    Frame {
        id: frame

        anchors.fill: parent

        HeaderLabel {
            id: title
            width: 96

            height: 24

            anchors.top: parent.top
            anchors.left: parent.left
        }

        BipolarSlider {
            id: panControls

            height: 40
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: title.bottom

            onMoved: root.panMoved(panControls.value)
        }

        LevelMeterFader {
            id: levelMeterFader

            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: channelControls.top
            anchors.top: panControls.bottom

            onFaderMoved: root.faderMoved(newVolume)
        }

        ChannelControls {
            id: channelControls
            height: 32
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            onMuteToggled: root.muteToggled()
            onSoloToggled: root.soloToggled()
        }

        Button {
            id: fxButton

            width: 24
            height: 36

            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: parent.top
            anchors.topMargin: -6

            text: qsTr("fx")

            padding: 6

            onClicked: root.fxButtonClicked()
        }
    }
}
