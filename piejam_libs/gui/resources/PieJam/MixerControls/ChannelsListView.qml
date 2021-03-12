// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ListView {
    id: root

    signal fxButtonClicked()
    signal addChannelClicked(string newChannelName)

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
        perform.onFxButtonClicked: root.fxButtonClicked()

        edit.model: model.item.edit
    }

    footer: Item {
        width: 152

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        visible: MixerViewSettings.mode == MixerViewSettings.edit

        ChannelAddStrip {
            id: channelAddStrip

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            name: privates.channelAddStripName

            onAddClicked: root.addChannelClicked(channelAddStrip.name)
        }
    }

    QtObject {
        id: privates

        property string channelAddStripName: ""
    }

    onCountChanged: privates.channelAddStripName = "In " + (root.count + 1)
}

