// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.15

Window {
    id: root

    property var modelFactory

    width: 800
    height: 480

    Material.theme: Material.Dark
    Material.primary: Material.Pink
    Material.accent: Material.Pink

    RootView {
        anchors.fill: parent

        modelFactory: root.modelFactory

        Component.onCompleted: root.visible = true
    }
}
