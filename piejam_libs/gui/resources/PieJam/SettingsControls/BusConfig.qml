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

    property bool mono: true
    property variant channels: [ "-" ]
    property alias name: nameTextField.text

    property alias monoChannelIndex: monoChannelSelect.currentIndex

    property alias stereoLeftChannelIndex: stereoLeftChannelSelect.currentIndex
    property alias stereoRightChannelIndex: stereoRightChannelSelect.currentIndex

    signal nameEdited(string name)
    signal monoChannelSelected(int ch)
    signal stereoLeftChannelSelected(int ch)
    signal stereoRightChannelSelected(int ch)
    signal deleteConfigClicked()

    id: root

    Frame {

        anchors.fill: parent

        TextField {
            id: nameTextField
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: stereoLeftChannelSelect.left
            anchors.rightMargin: 8
            anchors.left: parent.left

            onTextEdited: nameEdited(nameTextField.text)
        }

        ComboBox {
            id: monoChannelSelect
            width: 64 + 8 + 64
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: deleteConfig.left
            anchors.rightMargin: 8
            model: root.channels
            visible: root.mono

            onActivated: monoChannelSelected(index)
        }

        ComboBox {
            id: stereoLeftChannelSelect
            width: 64
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: stereoRightChannelSelect.left
            anchors.rightMargin: 8
            model: root.channels
            visible: !root.mono

            onActivated: stereoLeftChannelSelected(index)
        }

        ComboBox {
            id: stereoRightChannelSelect
            width: 64
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: deleteConfig.left
            anchors.rightMargin: 8
            model: root.channels
            visible: !root.mono

            onActivated: stereoRightChannelSelected(index)
        }

        Button {
            id: deleteConfig
            width: 38
            text: "X"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            onClicked: deleteConfigClicked()
        }
    }
}
