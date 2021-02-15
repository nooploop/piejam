// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ListView {
    id: root

    property bool soloActive: false

    signal soloToggled(int index)
    signal fxButtonClicked()

    spacing: 2
    clip: true
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    boundsMovement: Flickable.StopAtBounds
    reuseItems: true

    delegate: ChannelStrip {
        anchors.top: if (parent) parent.top
        anchors.bottom: if (parent) parent.bottom

        mono: model.item.mono
        levelLeft: model.item.levelLeft
        levelRight: model.item.levelRight
        meterMuted: (model.item.mute && !model.item.solo) || (root.soloActive && !model.item.solo)
        pan: model.item.panBalance
        volume: model.item.volume
        mute: model.item.mute
        solo: model.item.solo
        name: model.item.name

        volumeMidi.model: model.item.volumeMidi
        panMidi.model: model.item.panMidi
        muteMidi.model: model.item.muteMidi

        onFaderMoved: model.item.changeVolume(newVolume)
        onPanMoved: model.item.changePanBalance(newPan)
        onMuteToggled: model.item.changeMute(!model.item.mute)
        onSoloToggled: root.soloToggled(index)
        onFxButtonClicked: {
            model.item.focusFxChain()
            root.fxButtonClicked()
        }

        Binding {
            target: model.item
            property: "subscribed"
            value: visible
        }
    }
}

