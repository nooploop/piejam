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
        samplesPerPoint: 480
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

        anchors.fill: parent

        PJItems.Scope {
            id: leftScope

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: scopesLayout.height / 2

            lines: fxScope.leftLines
        }

        PJItems.Scope {
            id: rightScope

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: scopesLayout.height / 2

            lines: fxScope.rightLines
        }
    }

    Timer {
        interval: 16
        running: visible && root.content
        repeat: true
        onTriggered: root.content.scopeStream.requestUpdate()
    }
}
