// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQml 2.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    PJModels.FxScope {
        id: fxScope

        viewSize: scope.width
        samplesPerPoint: Math.pow(2, resolutionSlider.value)
    }

    onContentChanged: {
        if (root.content) {
            root.content.scopeStream.listener = fxScope.streamListener
        }
    }

    onVisibleChanged: fxScope.clear()

    onBypassedChanged: if (root.bypassed) fxScope.clear()

    PJItems.Scope {
        id: scope

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: resolutionSlider.top

        linesA: fxScope.leftLines
        linesAColor: Material.color(Material.Pink)

        linesB: fxScope.rightLines
        linesBColor: Material.color(Material.Blue)
    }

    Slider {
        id: resolutionSlider

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: channelASelector.top

        from: 0
        to: 11
        stepSize: 1
        value: 8
    }

    StereoChannelSelector {
        id: channelASelector

        name: "A"
        active: root.content ? root.content.activeA : false
        channel: root.content ? root.content.channelA : PJModels.StereoChannel.Left

        Material.accent: Material.Pink

        onActiveToggled: root.content.changeActiveA(!active)
        onChannelSelected: root.content.changeChannelA(ch)

        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

    Timer {
        interval: 16
        running: visible && root.content
        repeat: true
        onTriggered: root.content.scopeStream.requestUpdate()
    }
}
