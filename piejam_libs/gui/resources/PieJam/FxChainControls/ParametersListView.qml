// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

import ".."

Item {
    id: root

    property var content: null

    implicitWidth: 400

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }

    ListView {
        id: listView

        anchors.fill: parent

        spacing: 4
        clip: true
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds
        boundsMovement: Flickable.StopAtBounds

        model: if (root.content) root.content.parameters

        delegate: ParameterControl {
            paramModel: model.item

            height: parent ? parent.height : undefined

            ModelSubscription {
                target: model.item
                subscribed: visible
            }
        }
    }
}
