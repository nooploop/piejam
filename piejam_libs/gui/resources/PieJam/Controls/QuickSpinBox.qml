// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property int from: 0
    property int to: 99
    property int step: 1
    property int bigStep: 5
    property int value: 0

    property var textFromValue: function (value) { return value }

    implicitWidth: 96
    implicitHeight: 40

    Button {
        id: decButton

        text: "<"

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        width: height - 8

        enabled: value != from

        onClicked: decrement(step)

        function decrement(step) {
            root.value = Math.max(root.from, root.value - step)
        }
    }

    Label {
        text: textFromValue(value)
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        anchors.left: decButton.right
        anchors.right: incButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }

    Button {
        id: incButton

        text: ">"

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        width: height - 8

        enabled: value != to

        onClicked: increment(step)

        function increment(step) {
            root.value = Math.min(root.to, root.value + step)
        }
    }

    Timer {
        property int counter: 0

        running: decButton.pressed
        interval: 120
        repeat: true
        onTriggered: function () { decButton.decrement(counter > 10 ? bigStep : step); ++counter }
        onRunningChanged: counter = 0
    }

    Timer {
        property int counter: 0

        running: incButton.pressed
        interval: 120
        repeat: true
        onTriggered: function () { incButton.increment(counter > 10 ? bigStep : step); ++counter }
        onRunningChanged: counter = 0
    }
}
