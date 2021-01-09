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

    property real volume

    signal moved(real newVolume)

    implicitWidth: 58
    implicitHeight: 300

    Slider {
        id: slider

        anchors.fill: parent
        padding: 6

        value: MathExt.mapTo(dbScale.scaleData.dbToPosition(DbConvert.linToDb(root.volume)), privates.minPos, privates.maxPos, 0, 1)

        orientation: Qt.Vertical

        background: DbScale {
            id: dbScale

            anchors.fill: parent

            horizontalOrientation: DbScale.Orientation.Right
            backgroundColor: Material.backgroundColor

            scaleData: DbScaleData {
                DbScaleTick { position: privates.minPos; db: Number.NEGATIVE_INFINITY }
                DbScaleTick { position: 0.05; db: -60; dbStep: 10 }
                DbScaleTick { position: 0.35; db: -20; dbStep: 4 }
                DbScaleTick { position: 0.45; db: -12; dbStep: 3 }
                DbScaleTick { position: privates.maxPos; db: 12 }
            }
        }

        handle: Rectangle {
            x: slider.leftPadding + slider.availableWidth / 2 - width / 2
            y: slider.visualPosition * slider.availableHeight + height

            width: root.width
            height: 4

            color: ColorUtil.setAlpha(Material.accentColor, 0.6)
        }

        onMoved: {
            var newVolume = dbScale.scaleData.dbAt(MathExt.mapTo(slider.value, 0, 1, privates.minPos, privates.maxPos))
            root.moved(DbConvert.dbToLin(newVolume))
        }

        QtObject {
            id: privates

            property real minPos: slider.topPadding > root.height ? 0 : slider.topPadding / root.height
            property real maxPos: slider.bottomPadding > root.height ? 1 : (root.height - slider.bottomPadding) / root.height
        }
    }
}


