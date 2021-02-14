// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Controls.Material 2

Item {

    property alias model: statsList.model

    implicitWidth: 100
    implicitHeight: 20

    ListView {
        id: statsList

        anchors.fill: parent

        orientation: ListView.Horizontal

        spacing: 1

        delegate: Rectangle {
            width: statsList.width / statsList.model.length - statsList.spacing
            height: modelData * statsList.height
            y: statsList.height - height
            color: "#ff9900"
        }
    }
}
