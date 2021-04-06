// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0

Item {
    id: root

    property var parameters: []
    property var streams: []

    implicitWidth: 636

    ListView {
        id: scopes

        anchors.fill: parent

        boundsMovement: Flickable.StopAtBounds
        boundsBehavior: Flickable.StopAtBounds

        model: streams
        spacing: 4

        delegate: Item {
            width: scopes.width
            height: scopes.height

            FxScope {
                id: fxScope

                viewSize: leftScope.width
                samplesPerPoint: 480
            }

            PJItems.Scope {
                id: leftScope

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                height: parent.height / 2

                lines: fxScope.leftLines
            }

            PJItems.Scope {
                id: rightScope

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: leftScope.bottom

                lines: fxScope.rightLines
            }

            Binding {
                target: model.item
                property: "listener"
                value: fxScope.streamListener
            }

            Binding {
                target: model.item
                property: "subscribed"
                value: visible
            }

            Timer {
                interval: 16
                running: visible
                repeat: true
                onTriggered: model.item.requestUpdate()
            }
        }
    }
}
