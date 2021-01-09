// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    Item {

        anchors.fill: parent
        anchors.margins: 4

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
