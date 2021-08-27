// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property var content: null

    implicitWidth: listView.contentWidth

    ListView {
        id: listView

        width: contentWidth

        orientation: Qt.Horizontal

        spacing: 4

        model: if (root.content) root.content.parameters

        delegate: ParameterControl {
            paramModel: model.item

            height: root.height

            Binding {
                target: model.item
                property: "subscribed"
                value: visible
            }
        }
    }
}
