// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import "../Util/DbConvert.js" as DbConvert

Item {
    id: root

    property var peakLevel: null
    property var rmsLevel: null
    property bool muted: false
    property var scaleData

    implicitWidth: dbScaleLeft.width + indicatorLeft.width + indicatorRight.width + dbScaleRight.width + 3 * controls.spacing
    implicitHeight: 300

    QtObject {
        id: private_

        readonly property real peakLevelLeft: root.peakLevel ? root.peakLevel.levelLeft : 0
        readonly property real peakLevelRight: root.peakLevel ? root.peakLevel.levelRight : 0

        readonly property real rmsLevelLeft: root.rmsLevel ? root.rmsLevel.levelLeft : 0
        readonly property real rmsLevelRight: root.rmsLevel ? root.rmsLevel.levelRight : 0
    }

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor
    }

    Gradient {
        id: levelGradient

        GradientStop { position: 0; color: "#ff0000" }
        GradientStop { position: 0.0475; color: "#ffa500" }
        GradientStop { position: 0.19; color: "#ffff00" }
        GradientStop { position: 0.285; color: "#7ee00d" }
        GradientStop { position: 0.95; color: "#7ee00d" }
        GradientStop { position: 1; color: "#008000" }
    }

    Gradient {
        id: mutedLevelGradient

        GradientStop { position: 0; color: "#c0c0c0" }
        GradientStop { position: 1; color: "#909090" }
    }

    Row {
        id: controls

        anchors.fill: parent

        spacing: 1

        DbScale {
            id: dbScaleLeft

            height: parent.height

            padding: 6

            orientation: DbScale.Orientation.Right
            backgroundColor: Material.backgroundColor

            scaleData: root.scaleData
        }

        LevelIndicator {
            id: indicatorLeft

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6

            peakLevel: scaleData ? scaleData.dBToPosition(DbConvert.linToDb(private_.peakLevelLeft)) : 0
            rmsLevel: scaleData ? scaleData.dBToPosition(DbConvert.linToDb(private_.rmsLevelLeft)) : 0

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        LevelIndicator {
            id: indicatorRight

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6

            peakLevel: scaleData ? scaleData.dBToPosition(DbConvert.linToDb(private_.peakLevelRight)) : 0
            rmsLevel: scaleData ? scaleData.dBToPosition(DbConvert.linToDb(private_.rmsLevelRight)) : 0

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        DbScale {
            id: dbScaleRight

            height: parent.height

            padding: 6

            backgroundColor: Material.backgroundColor

            scaleData: root.scaleData
            withText: false
        }
    }
}
