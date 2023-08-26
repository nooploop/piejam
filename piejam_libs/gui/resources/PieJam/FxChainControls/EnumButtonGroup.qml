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

    model: root.paramModel ? root.paramModel.values : null

    delegate: Button {
        width: root.buttonWidth
        height: root.buttonHeight

        text: model.text

        checkable: true
        autoExclusive: true

        checked: model.value === root.paramModel.value

        onClicked: root.paramModel.changeValue(model.value)
    }
}
