// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import ".."

SubscribableItem {
    id: root

    implicitWidth: 400

    ListView {
        id: listView

        anchors.fill: parent

        spacing: 4
        clip: true
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        model: root.model ? root.model.parameters : null

        delegate: ParameterControl {
            paramModel: model.item

            height: parent ? parent.height : undefined
        }
    }
}
