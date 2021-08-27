// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

import ".."
import "../Controls"

Repeater {
    id: root

    property var paramModel: null
    property var names: []

    anchors.fill: parent

    model: root.paramModel ? root.paramModel.maxValue - root.paramModel.minValue : 0

    delegate: Button {
        property int value: modelData + root.paramModel.minValue

        text: root.paramModel.intValueToString(value)

        checkable: true
        autoExclusive: true

        checked: value === root.paramModel.value

        onClicked: root.paramModel.changeValue(value)
    }
}
