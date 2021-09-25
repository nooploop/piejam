// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property alias sourceComponent: loader.sourceComponent

    implicitWidth: loader.item ? loader.item.implicitWidth : 200

    BusyIndicator {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        running: loader.status != Loader.Ready
        visible: loader.status != Loader.Ready
    }

    Loader {
        id: loader

        anchors.fill: parent

        asynchronous: true

        visible: status == Loader.Ready
    }
}
