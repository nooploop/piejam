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

    signal addClicked()

    implicitWidth: 150
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        Button {
            id: addButton

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            text: qsTr("Add")

            Material.background: Material.Green

            onClicked: root.addClicked()
        }
    }
}
