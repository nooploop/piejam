// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import PieJam.Items 1.0 as PJItems

import ".."
import "../Controls"
import "../Util/ColorExt.js" as ColorExt
import "../Util/DbConvert.js" as DbConvert
import "../Util/MathExt.js" as MathExt

SubscribableItem {
    id: root

    property var scaleData: null
    property bool muted: false

    signal moved()

    implicitWidth: 50
    implicitHeight: 300

    QtObject {
        id: private_

        readonly property real normValue: root.model ? root.model.normalizedValue : 0
    }

    RowLayout {
        anchors.fill: parent

        spacing: 2

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Repeater {
                model: root.scaleData ? root.scaleData.ticks : null

                delegate: Label {
                    anchors.left: parent ? parent.left : undefined
                    anchors.right: parent ? parent.right : undefined

                    y: (1 - modelData.normalized) * (parent.height - 12)

                    font.pixelSize: 10
                    font.bold: true

                    color: root.muted ? Material.secondaryTextColor : Material.primaryTextColor

                    text: modelData.dB
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        PJItems.DbScale {
            id: dbScale

            Layout.preferredWidth: 8
            Layout.fillHeight: true

            scaleData: root.scaleData
            color: root.muted ? Material.secondaryTextColor : Material.primaryTextColor
            tickOffset: 6
            edge: PJItems.DbScale.Edge.Right
        }
    }

    MouseArea {
        id: touchArea

        property real inc: 1 / 250
        property real lastY: 0

        anchors.fill: parent

        preventStealing: true

        onPressed: {
            lastY = mouse.y

            mouse.accepted = true
        }

        onPositionChanged: {
            var newNormValue = MathExt.clamp(private_.normValue + (lastY - mouse.y) / height, 0, 1)
            root.model.changeNormalizedValue(newNormValue)
            lastY = mouse.y
            Info.showParameterValue(root.model)
            root.moved()
        }

        onDoubleClicked: {
            if (root.model) {
                root.model.resetToDefault()
                Info.showParameterValue(root.model)
            }
        }
    }

    Rectangle {
        id: handle

        y: (1 - private_.normValue) * (root.height - 12) + 6 - Screen.devicePixelRatio

        anchors.left: parent.left
        anchors.right: parent.right

        height: 2 + Screen.devicePixelRatio

        color: ColorExt.setAlpha(Material.accentColor, 0.6)
    }

    MidiAssignArea {
        id: midiAssign

        anchors.fill: parent

        model: root.model ? root.model.midi : null
    }
}


