// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property alias name: nameText.text

    signal addMonoClicked()
    signal addStereoClicked()

    implicitWidth: 132
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        TextField {
            id: nameText

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            placeholderText: qsTr("Name")
        }

        Button {
            id: addMonoButton

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: addStereoButton.top

            text: qsTr("Add Mono")

            Material.background: Material.color(Material.Green, Material.Shade400)

            onClicked: root.addMonoClicked()
        }

        Button {
            id: addStereoButton

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            text: qsTr("Add Stereo")

            Material.background: Material.color(Material.Green, Material.Shade400)

            onClicked: root.addStereoClicked()
        }
    }
}
