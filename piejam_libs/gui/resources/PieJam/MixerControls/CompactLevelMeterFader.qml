// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../Controls/Color.js" as ColorUtil
import "../Util/DbConvert.js" as DbConvert
import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real volume: 0
    property alias levelLeft: levelMeter.levelLeft
    property alias levelRight: levelMeter.levelRight

    signal faderMoved(real newVolume)

    implicitWidth: levelMeter.width
    implicitHeight: 300

    DbScaleData {
        id: scaleData

        DbScaleTick { position: privates.minPos; db: Number.NEGATIVE_INFINITY }
        DbScaleTick { position: 0.05; db: -48; dbStep: 6 }
        DbScaleTick { position: privates.maxPos; db: 12 }
    }

    Slider {
        id: slider

        anchors.fill: parent

        padding: 6

        value: MathExt.mapTo(scaleData.dbToPosition(DbConvert.linToDb(root.volume)),
                             privates.minPos,
                             privates.maxPos,
                             0,
                             1)

        orientation: Qt.Vertical

        background: LevelMeter {
            id: levelMeter
        }

        handle: Rectangle {
            x: slider.leftPadding + slider.availableWidth / 2 - width / 2
            y: slider.visualPosition * slider.availableHeight + height

            width: parent.width
            height: 4

            color: ColorUtil.setAlpha(Material.accentColor, 0.6)
        }

        onMoved: {
            var newVolume = scaleData.dbAt(MathExt.mapTo(slider.value, 0, 1, privates.minPos, privates.maxPos))
            root.faderMoved(DbConvert.dbToLin(newVolume))
        }

        QtObject {
            id: privates

            property real minPos: slider.topPadding > root.height ? 0 : slider.topPadding / root.height
            property real maxPos: slider.bottomPadding > root.height ? 1 : (root.height - slider.bottomPadding) / root.height
        }
    }
}
