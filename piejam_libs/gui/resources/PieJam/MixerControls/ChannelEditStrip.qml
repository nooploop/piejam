// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."

Item {
    id: root

    property alias name: nameText.text
    property bool deletable: true

    signal deleteClicked()
    signal nameEdited(string newName)

    implicitWidth: 150
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        Button {
            id: deleteButton

            visible: root.deletable
            enabled: root.deletable

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            text: qsTr("Delete")

            Material.background: Material.Red

            onClicked: root.deleteClicked()
        }

        TextField {
            id: nameText

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            placeholderText: qsTr("Name")

            onTextEdited: root.nameEdited(nameText.text)
        }

        Label {
            id: audioInLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: moveButtonsRow.bottom
            anchors.topMargin: 8

            text: qsTr("Audio In")
        }

        ComboBox {
            id: audioInSelect

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioInLabel.bottom
        }

        Label {
            id: audioOutLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioInSelect.bottom
            anchors.topMargin: 8

            text: qsTr("Audio Out")
        }

        ComboBox {
            id: audioOutSelect

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioOutLabel.bottom
        }

        Row {
            id: moveButtonsRow

            height: 48

            anchors.top: nameText.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 8

            Button {
                id: moveLeftButton

                width: 48
                height: 48

                text: qsTr("<")
            }

            Button {
                id: moveRightButton

                width: 48
                height: 48

                text: qsTr(">")
            }
        }
    }
}
