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


Item {

    id: root

    property alias nameLabelWidth: nameLabel.width
    property alias nameLabelText: nameLabel.text
    property alias model: comboBox.model
    property alias currentIndex: comboBox.currentIndex

    signal optionSelected(int index)

    Frame {
        id: frame

        anchors.fill: parent

        Label {
            id: nameLabel

            width: 128
            anchors.verticalCenter: parent.verticalCenter

            anchors.left: parent.left

            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 18
        }

        ComboBox {
            id: comboBox
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 8

            anchors.right: parent.right
            anchors.left: nameLabel.right

            onActivated: optionSelected(index)
        }
    }

}
