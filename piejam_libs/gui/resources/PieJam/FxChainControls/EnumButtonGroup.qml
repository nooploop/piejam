// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

Repeater {
    id: root

    property var paramModel: null
    property int buttonWidth: 64
    property int buttonHeight: 48

    model: root.paramModel ? root.paramModel.maxValue - root.paramModel.minValue + 1 : 0

    delegate: Button {
        property int value: modelData + root.paramModel.minValue

        width: root.buttonWidth
        height: root.buttonHeight

        text: root.paramModel.valueToString(value)

        checkable: true
        autoExclusive: true

        checked: value === root.paramModel.value

        onClicked: root.paramModel.changeValue(value)
    }
}
