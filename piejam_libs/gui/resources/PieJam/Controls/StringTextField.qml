// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

import ".."

SubscribableItem {
    id: root

    implicitWidth: item.implicitWidth
    implicitHeight: item.implicitHeight

    TextField {
        id: item

        anchors.fill: parent

        text: root.model ? root.model.value : ""

        onEditingFinished: {
            root.model.changeValue(text)
            focus = false
        }
    }
}
