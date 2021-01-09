// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

Label {
    id: root

    property var backgroundColor: Material.primaryColor

    padding: 2
    leftPadding: 4
    rightPadding: 4

    verticalAlignment: Text.AlignVCenter
    font.bold: true
    font.pixelSize: 12

    elide: Label.ElideRight

    background: Rectangle {
        color: root.backgroundColor
        radius: 2
    }
}
