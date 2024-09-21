// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property alias diskUsage: diskSpaceIndicator.usage
    property real audioLoad: 0
    property int xruns: 0
    property var cpuLoad: ({})
    property int cpuTemp: 0

    implicitWidth: 96

    Rectangle {
        id: readoutLabel

        anchors.fill: parent

        color: Material.background
        radius: 4

        DiskSpaceIndicator {
            id: diskSpaceIndicator

            anchors.left: parent.left
        }

        Label {
            id: audioLoadLabel

            height: 16

            anchors.right: xrunsLabel.left
            anchors.top: parent.top

            padding: 2
            leftPadding: 4
            rightPadding: 4
            horizontalAlignment: Text.AlignRight
            textFormat: Text.PlainText
            text: (root.audioLoad * 100).toFixed(1)
        }

        Label {
            id: xrunsLabel

            width: 16
            height: 16

            anchors.right: parent.right
            anchors.top: parent.top

            padding: 2
            rightPadding: 4
            text: "%"
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.PlainText

            color: root.xruns === 0 ? "#00ff00" : "#ffff00"

            ColorAnimation {
                id: xrunFlash

                target: xrunsLabel
                property: "color"
                from: "#ff0000"
                to: "#ffff00"
                duration: 5000
            }

            Connections {
                target: root

                function onXrunsChanged() {
                    if (root.xruns !== 0)
                        xrunFlash.restart()
                    else {
                        xrunFlash.stop()
                        xrunsLabel.color = "#00ff00"
                    }
                }
            }
        }

        Label {
            id: cpuTempLabel

            width: 32

            anchors.right: cpuLoadMeter.left
            anchors.top: audioLoadLabel.bottom
            anchors.bottom: parent.bottom

            text: root.cpuTemp + "°C"
            font.pixelSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom
            textFormat: Text.PlainText
        }

        CpuLoadMeter {
            id: cpuLoadMeter

            width: 32

            anchors.right: parent.right
            anchors.top: audioLoadLabel.bottom
            anchors.bottom: parent.bottom
            anchors.margins: 2

            model: root.cpuLoad
        }
    }

}
