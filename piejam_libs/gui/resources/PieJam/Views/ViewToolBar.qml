// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    default property alias toolbarButtons: buttons.children

    implicitWidth: 48

    color: Material.color(Material.Grey, Material.Shade800)

    ColumnLayout {
        id: buttons

        anchors.fill: parent
    }
}
