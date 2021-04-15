// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQml 2.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    FxScope {
        id: fxScope

        viewSize: leftScope.width
        samplesPerPoint: Math.pow(2, resolutionSlider.value)
    }

    onContentChanged: {
        if (root.content) {
            root.content.scopeStream.listener = fxScope.streamListener
        }
    }

    onVisibleChanged: fxScope.clear()

    onBypassedChanged: if (root.bypassed) fxScope.clear()

    ColumnLayout {
        id: scopesLayout

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: resolutionSlider.top

        PJItems.Scope {
            id: leftScope

            Layout.fillWidth: true
            Layout.fillHeight: true

            lines: fxScope.leftLines
        }

        PJItems.Scope {
            id: rightScope

            Layout.fillWidth: true
            Layout.fillHeight: true

            lines: fxScope.rightLines
        }
    }

    Slider {
        id: resolutionSlider

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        from: 0
        to: 11
        stepSize: 1
        value: 8
    }

    Timer {
        interval: 16
        running: visible && root.content
        repeat: true
        onTriggered: root.content.scopeStream.requestUpdate()
    }
}
