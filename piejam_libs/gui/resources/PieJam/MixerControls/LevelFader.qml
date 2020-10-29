// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "DbConvert.js" as DbConvert
import "../Controls/Color.js" as ColorUtil
import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real volume

    signal moved(real newVolume)

    width: 58
    height: 280

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
            implicitWidth: root.width
            implicitHeight: 4
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


