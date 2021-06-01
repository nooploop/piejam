// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtGraphicalEffects 1.15

Item {
    id: root

    property int usage: 0

    implicitWidth: 32
    implicitHeight: 32

    Image {
        id: icon

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: usageLabel.top
        anchors.margins: 2

        source: "qrc:///images/icons/micro-sd.svg"
        fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
        anchors.fill: icon
        source: icon
        color: "white"
        antialiasing: true
    }

    Label {
        id: usageLabel

        height: 8

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        text: root.usage != 100 ? root.usage + "%" : qsTr("FULL")
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 8
        color: Qt.lighter(Qt.rgba(root.usage / 100., (100 - root.usage) / 100., 0, 1))
    }
}
