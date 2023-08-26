// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    property var paramModel: null

    ComboBox {
        id: comboBox

        anchors.fill: parent;

        model: root.paramModel ? root.paramModel.values : null
        currentIndex: root.paramModel ? root.paramModel.value - root.paramModel.minValue : -1
        textRole: "text"
        valueRole: "value"

        onActivated: {
            if (root.paramModel)
                root.paramModel.changeValue(root.paramModel.minValue + index)
        }
    }
}
