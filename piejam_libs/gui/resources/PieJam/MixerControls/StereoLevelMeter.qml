// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Items 1.0 as PJItems

import ".."
import "../Util/DbConvert.js" as DbConvert

Item {
    id: root

    property var peakLevel: null
    property var rmsLevel: null
    property bool muted: false
    property var scaleData: null

    implicitWidth: 56
    implicitHeight: 300

    QtObject {
        id: private_

        readonly property real peakLevelLeft: root.peakLevel ? root.peakLevel.levelLeft : 0
        readonly property real peakLevelRight: root.peakLevel ? root.peakLevel.levelRight : 0

        readonly property real rmsLevelLeft: root.rmsLevel ? root.rmsLevel.levelLeft : 0
        readonly property real rmsLevelRight: root.rmsLevel ? root.rmsLevel.levelRight : 0
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

    RowLayout {
        id: controls

        anchors.fill: parent

        spacing: 2

        Item {
            Layout.preferredWidth: 20
            Layout.fillHeight: true

            Repeater {
                model: root.scaleData ? root.scaleData.ticks : null

                delegate: Label {
                    anchors.left: parent ? parent.left : undefined
                    anchors.right: parent ? parent.right : undefined

                    y: (1 - modelData.position) * (parent.height - 12)

                    font.pixelSize: 10
                    font.bold: true

                    color: root.muted ? Material.secondaryTextColor : Material.primaryTextColor

                    text: modelData.dB
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        PJItems.DbScale {
            id: dbScaleLeft

            Layout.preferredWidth: 8
            Layout.fillHeight: true

            scaleData: root.scaleData
            color: root.muted ? Material.secondaryTextColor : Material.primaryTextColor
            tickOffset: 6
            edge: PJItems.DbScale.Edge.Right
        }

        LevelIndicator {
            id: indicatorLeft

            Layout.preferredWidth: 6
            Layout.fillHeight: true
            Layout.topMargin: 6
            Layout.bottomMargin: 6

            peakLevel: root.scaleData ? root.scaleData.dBToPosition(DbConvert.to_dB(private_.peakLevelLeft)) : 0
            rmsLevel: root.scaleData ? root.scaleData.dBToPosition(DbConvert.to_dB(private_.rmsLevelLeft)) : 0

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        LevelIndicator {
            id: indicatorRight

            Layout.preferredWidth: 6
            Layout.fillHeight: true
            Layout.topMargin: 6
            Layout.bottomMargin: 6

            peakLevel: root.scaleData ? root.scaleData.dBToPosition(DbConvert.to_dB(private_.peakLevelRight)) : 0
            rmsLevel: root.scaleData ? root.scaleData.dBToPosition(DbConvert.to_dB(private_.rmsLevelRight)) : 0

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        PJItems.DbScale {
            id: dbScaleRight

            Layout.preferredWidth: 8
            Layout.fillHeight: true

            scaleData: root.scaleData
            color: root.muted ? Material.secondaryTextColor : Material.primaryTextColor
            tickOffset: 6
        }
    }

    ModelSubscription {
        target: root.peakLevel
        subscribed: root.visible
    }

    ModelSubscription {
        target: root.rmsLevel
        subscribed: root.visible
    }
}
