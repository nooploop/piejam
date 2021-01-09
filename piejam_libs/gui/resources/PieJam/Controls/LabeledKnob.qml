// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

Item {
    property alias bipolar: knob.bipolar
    property alias text: label.text
    property alias value: knob.value
    property alias labelVisible: label.visible

    signal moved()

    id: root

    Knob {
        id: knob

        anchors.bottomMargin: 2
        anchors.bottom: root.labelVisible ? label.top : parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        onMoved: root.moved()
    }

    Label {
        id: label

        text: knob.value.toFixed(2)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
