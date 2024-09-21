// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias name: nameText.text

    signal addMonoClicked()
    signal addStereoClicked()

    implicitWidth: 132

    Frame {
        id: frame

        anchors.fill: parent

        ColumnLayout {

            anchors.fill: parent

            TextField {
                id: nameText

                Layout.fillWidth: true

                placeholderText: qsTr("Name")
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Button {
                id: addMonoButton

                Layout.fillWidth: true

                text: qsTr("+ Mono")

                Material.background: Material.color(Material.Green, Material.Shade400)

                onClicked: root.addMonoClicked()
            }

            Button {
                id: addStereoButton

                Layout.fillWidth: true

                text: qsTr("+ Stereo")

                Material.background: Material.color(Material.Green, Material.Shade400)

                onClicked: root.addStereoClicked()
            }
        }
    }
}
