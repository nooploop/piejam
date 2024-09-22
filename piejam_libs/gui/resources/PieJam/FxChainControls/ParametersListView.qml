// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import ".."

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 400

    ListView {
        id: listView

        anchors.fill: parent

        spacing: 4
        clip: true
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        model: root.model ? root.model.parametersList : null

        delegate: ParameterControl {
            paramModel: model.item

            height: parent ? parent.height : undefined
        }
    }
}
