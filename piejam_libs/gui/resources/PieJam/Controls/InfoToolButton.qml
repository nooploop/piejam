// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ToolButton {
    id: root

    property alias info: infoLabel.text

    Label {
        id: infoLabel

        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6

        z: 1

        background: Rectangle {
            id: backgroundRect

            color: Material.primaryColor
            radius: 4
        }
    }
}
