// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import "../Util/ColorExt.js" as ColorExt

Item {
    id: root

    property real peakLevel: 1
    property real rmsLevel: 1
    property alias gradient: backgroundRect.gradient
    property color fillColor: "#000000"

    width: 40
    height: 200

    QtObject {
        id: private_

        readonly property color peakFillColor: ColorExt.setAlpha(root.fillColor, 0.5)
    }

    Rectangle {
        id: backgroundRect

        anchors.fill: parent

        Rectangle {
            z: 1

            color: private_.peakFillColor

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: (1 - root.rmsLevel) * parent.height
        }

        Rectangle {
            z: 2

            color: root.fillColor

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: (1 - Math.max(root.peakLevel, root.rmsLevel)) * parent.height
        }
    }
}
