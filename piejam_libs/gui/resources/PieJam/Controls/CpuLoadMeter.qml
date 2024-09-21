// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15

Item {
    id: root

    property alias model: statsList.model

    implicitWidth: 100
    implicitHeight: 20

    ListView {
        id: statsList

        anchors.fill: parent

        orientation: ListView.Horizontal

        spacing: 1

        delegate: Rectangle {
            y: statsList.height - height

            width: statsList.width / statsList.model.length - statsList.spacing
            height: modelData * statsList.height

            color: "#ff9900"
        }
    }
}
