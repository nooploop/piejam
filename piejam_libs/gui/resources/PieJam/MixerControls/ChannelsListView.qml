// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Models 1.0 as PJModels

ListView {
    id: root

    spacing: 2
    clip: true
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    boundsMovement: Flickable.StopAtBounds

    delegate: ChannelStrip {
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        perform: model.item.perform
        edit: model.item.edit
        fx: model.item.fx
    }
}

