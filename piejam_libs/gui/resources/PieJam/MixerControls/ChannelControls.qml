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
import QtQuick.Layouts 1.12

Item {
    id: root

    property alias soloEnabled: soloButton.enabled
    property alias solo: soloButton.checked
    property alias mute: muteButton.checked

    signal soloToggled()
    signal muteToggled()

    Button {
        id: soloButton
        width: 40
        height: 40
        text: qsTr("S")
        anchors.right: muteButton.left
        anchors.rightMargin: 8

        checkable: true

        Material.accent: Material.Yellow

        onToggled: root.soloToggled()
    }

    Button {
        id: muteButton
        width: 40
        height: 40
        text: qsTr("M")
        anchors.right: parent.right

        checkable: true

        onToggled: root.muteToggled()
    }
}
