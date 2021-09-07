// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ListView {
    id: root

    signal fxButtonClicked(int index)

    spacing: 2
    clip: true
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    boundsMovement: Flickable.StopAtBounds
    reuseItems: true

    delegate: ChannelStrip {
        anchors.top: if (parent) parent.top
        anchors.bottom: if (parent) parent.bottom

        perform.model: model.item.perform
        perform.onFxButtonClicked: root.fxButtonClicked(model.index)

        edit.model: model.item.edit
    }
}

