// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0

import ".."
import "../Controls"

Item {
    id: root

    property var modelX: null
    property var modelY: null

    QtObject {
        id: private_

        readonly property var paramModelX: root.modelX && root.modelX.type === Parameter.Type.Float ? root.modelX : null
        readonly property real valueX: private_.paramModelX ? private_.paramModelX.normalizedValue : 0

        readonly property var paramModelY: root.modelY && root.modelY.type === Parameter.Type.Float ? root.modelY : null
        readonly property real valueY: private_.paramModelY ? 1 - private_.paramModelY.normalizedValue : 0
    }

    XYPad {
        anchors.fill: parent

        posX: private_.valueX
        posY: private_.valueY

        handleRadius: 6
        handleColor: Material.color(Material.Yellow, Material.Shade400)

        onChangePos: {
            if (private_.paramModelX)
                private_.paramModelX.changeNormalizedValue(x)

            if (private_.paramModelY)
                private_.paramModelY.changeNormalizedValue(1 - y)
        }
    }

    ModelSubscription {
        target: root.modelX
        subscribed: root.visible
    }

    ModelSubscription {
        target: root.modelY
        subscribed: root.visible
    }
}
