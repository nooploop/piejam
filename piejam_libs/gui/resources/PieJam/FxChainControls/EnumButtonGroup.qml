// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

Repeater {
    id: root

    property var paramModel: null
    property int buttonImplicitWidth: 64
    property int buttonImplicitHeight: 48
    property var icons: null

    model: root.paramModel ? root.paramModel.values : null

    delegate: Button {
        implicitWidth: root.buttonImplicitWidth
        implicitHeight: root.buttonImplicitHeight

        text: model.text

        checkable: true
        autoExclusive: true

        icon.source: root.icons ? root.icons[model.index] : ""

        display: icons ? AbstractButton.IconOnly : AbstractButton.TextOnly

        checked: model.value === root.paramModel.value

        onClicked: root.paramModel.changeValue(model.value)
    }
}
