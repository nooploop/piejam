// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    property bool mono: true
    property variant channels: [ "-" ]
    property alias name: nameTextField.text

    property int monoChannelIndex: 0
    property int stereoLeftChannelIndex: 0
    property int stereoRightChannelIndex: 0

    signal nameEdited(string name)
    signal monoChannelSelected(int ch)
    signal stereoLeftChannelSelected(int ch)
    signal stereoRightChannelSelected(int ch)
    signal deleteConfigClicked()

    id: root

    RowLayout {

        anchors.fill: parent
        anchors.margins: 4

        TextField {
            id: nameTextField

            Layout.fillWidth: true

            onEditingFinished: nameEdited(nameTextField.text)
        }

        ComboBox {
            id: monoChannelSelect

            Layout.preferredWidth: 64 + 6 + 64

            model: root.channels
            currentIndex: root.monoChannelIndex
            visible: root.mono

            onActivated: monoChannelSelected(index)
            onModelChanged: currentIndex = Qt.binding(function() { return root.monoChannelIndex })
        }

        ComboBox {
            id: stereoLeftChannelSelect

            Layout.preferredWidth: 64

            model: root.channels
            currentIndex: root.stereoLeftChannelIndex
            visible: !root.mono

            onActivated: stereoLeftChannelSelected(index)
            onModelChanged: currentIndex = Qt.binding(function() { return root.stereoLeftChannelIndex })
        }

        ComboBox {
            id: stereoRightChannelSelect

            Layout.preferredWidth: 64

            model: root.channels
            currentIndex: root.stereoRightChannelIndex
            visible: !root.mono

            onActivated: stereoRightChannelSelected(index)
            onModelChanged: currentIndex = Qt.binding(function() { return root.stereoRightChannelIndex })
        }

        Button {
            id: deleteConfig

            Layout.preferredWidth: 38

            text: "X"

            onClicked: deleteConfigClicked()
        }
    }
}
