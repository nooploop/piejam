// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Window 2.15

Window {
    id: root

    property var modelManager: null

    width: 800
    height: 480

    RootView {
        anchors.fill: parent

        modelManager: root.modelManager

        Component.onCompleted: root.visible = true
    }
}
