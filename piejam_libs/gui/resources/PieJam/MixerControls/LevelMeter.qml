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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

import "DbConvert.js" as DbConvert

Rectangle {
    id: root

    property real level

    width: 64
    height: 300
    color: Material.backgroundColor

    DbScaleData {
        id: meterScaleData

        DbScaleTick { position: privates.minPos; db: Number.NEGATIVE_INFINITY }
        DbScaleTick { position: 0.05; db: -60; dbStep: 6 }
        DbScaleTick { position: privates.maxPos; db: 0 }
    }

    DbScale {
        id: dbScaleLeft
        anchors.right: indicator.left
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        horizontalOrientation: DbScale.Orientation.Right
        backgroundColor: Material.backgroundColor

        scaleData: meterScaleData
    }

    LevelIndicator {
        id: indicator
        width: 12
        anchors.bottomMargin: 6
        anchors.topMargin: 6
        anchors.right: dbScaleRight.left
        anchors.bottom: parent.bottom
        anchors.top: parent.top

        level: DbConvert.mapTo(meterScaleData.dbToPosition(DbConvert.linToDb(root.level)), privates.minPos, privates.maxPos, 0, 1)

        gradient: Gradient {
            GradientStop { position: 0; color: "#ff0000" }
            GradientStop { position: DbConvert.mapTo(1 - meterScaleData.dbToPosition(-3), privates.minPos, privates.maxPos, 0, 1); color: "#ffa500" }
            GradientStop { position: DbConvert.mapTo(1 - meterScaleData.dbToPosition(-12), privates.minPos, privates.maxPos, 0, 1); color: "#ffff00" }
            GradientStop { position: DbConvert.mapTo(1 - meterScaleData.dbToPosition(-20), privates.minPos, privates.maxPos, 0, 1); color: "#7ee00d" }
            GradientStop { position: 0.95; color: "#7ee00d" }
            GradientStop { position: 1; color: "#008000" }
        }

        fillColor: Material.backgroundColor
    }

    DbScale {
        id: dbScaleRight
        width: 12
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        backgroundColor: Material.backgroundColor

        scaleData: meterScaleData
        enableText: false
    }

    QtObject {
        id: privates

        property real minPos: 6 / root.height
        property real maxPos: (root.height - 6) / root.height
    }
}
