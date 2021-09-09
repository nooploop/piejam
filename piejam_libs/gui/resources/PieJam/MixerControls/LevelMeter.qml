// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../Util/DbConvert.js" as DbConvert
import "../Util/MathExt.js" as MathExt

Item {
    id: root

    property real levelLeft: 0
    property real levelRight: 0
    property bool muted: false
    property int indicatorPadding: 6

    implicitWidth: dbScaleLeft.width + indicatorLeft.width + indicatorRight.width + dbScaleRight.width
    implicitHeight: 300

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor
    }

    DbScaleData {
        id: meterScaleData

        DbScaleTick { position: privates.minPos; db: Number.NEGATIVE_INFINITY }
        DbScaleTick { position: 0.05; db: -60; dbStep: 6 }
        DbScaleTick { position: privates.maxPos; db: 0 }

        function levelConv(db) {
            const srcDelta = 60
            const dstMin = MathExt.mapTo(0.05, privates.minPos, privates.maxPos, 0, 1)
            const dstDelta = 1 - dstMin
            const factor = dstDelta / srcDelta
            return db < -60 ? 0 : (db + 60) * factor + dstMin
        }
    }

    Gradient {
        id: levelGradient

        GradientStop { position: 0; color: "#ff0000" }
        GradientStop { position: MathExt.mapTo(1 - meterScaleData.dbToPosition(-3),  privates.minPos, privates.maxPos, 0, 1); color: "#ffa500" }
        GradientStop { position: MathExt.mapTo(1 - meterScaleData.dbToPosition(-12), privates.minPos, privates.maxPos, 0, 1); color: "#ffff00" }
        GradientStop { position: MathExt.mapTo(1 - meterScaleData.dbToPosition(-20), privates.minPos, privates.maxPos, 0, 1); color: "#7ee00d" }
        GradientStop { position: 0.95; color: "#7ee00d" }
        GradientStop { position: 1; color: "#008000" }
    }

    Gradient {
        id: mutedLevelGradient

        GradientStop { position: 0; color: "#c0c0c0" }
        GradientStop { position: 1; color: "#909090" }
    }

    Row {
        anchors.fill: parent

        DbScale {
            id: dbScaleLeft

            height: parent.height

            horizontalOrientation: DbScale.Orientation.Right
            backgroundColor: Material.backgroundColor

            scaleData: meterScaleData
        }

        LevelIndicator {
            id: indicatorLeft

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.indicatorPadding
            anchors.top: parent.top
            anchors.topMargin: root.indicatorPadding

            level: meterScaleData.levelConv(root.levelLeft)

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        LevelIndicator {
            id: indicatorRight

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.indicatorPadding
            anchors.top: parent.top
            anchors.topMargin: root.indicatorPadding

            level: meterScaleData.levelConv(root.levelRight)

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        DbScale {
            id: dbScaleRight

            height: parent.height

            backgroundColor: Material.backgroundColor

            scaleData: meterScaleData
            enableText: false
        }
    }

    QtObject {
        id: privates

        property real minPos: root.height < root.indicatorPadding ? 0 : root.indicatorPadding / root.height
        property real maxPos: root.height < root.indicatorPadding ? 1 : (root.height - root.indicatorPadding) / root.height
    }
}
