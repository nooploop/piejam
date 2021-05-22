// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.0
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import ".."
import "../Controls"

Item {
    id: root

    property var model

    implicitWidth: 800
    implicitHeight: 48

    Rectangle {
        id: statusBarBackground

        anchors.fill: parent

        color: Material.color(Material.Grey, Material.Shade800)

        Label {
            id: infoMessageLabel

            anchors.left: parent.left
            anchors.right: recordButton.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6

            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            clip: true

            padding: 2
            leftPadding: 4
            rightPadding: 4
            text: Info.message

            background: Rectangle {
                color: Material.background
                radius: 4
            }
        }

        Button {
            id: recordButton

            width: 48

            anchors.right: midiLearn.left
            anchors.rightMargin: 6

            checkable: true
            Material.accent: Material.Red

            text: "REC"
        }

        Button {
            id: midiLearn

            width: 38

            anchors.right: readoutLabel.left
            anchors.rightMargin: 6

            checkable: true

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/midi-port.svg"
            display: AbstractButton.IconOnly

            Binding {
                target: MidiLearn
                property: "active"
                value: midiLearn.checked
            }
        }

        Rectangle {
            id: readoutLabel

            width: 64

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6

            color: Material.background
            radius: 4

            Label {
                id: audioLoadLabel

                height: 16

                anchors.left: parent.left
                anchors.right: xrunsLabel.left
                anchors.top: parent.top

                padding: 2
                leftPadding: 4
                rightPadding: 4
                horizontalAlignment: Text.AlignRight
                textFormat: Text.PlainText
                text: (root.model.audioLoad * 100).toFixed(1)
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

                color: root.model.xruns === 0 ? "#00ff00" : "#ffff00"

                ColorAnimation {
                    id: xrunFlash

                    target: xrunsLabel
                    property: "color"
                    from: "#ff0000"
                    to: "#ffff00"
                    duration: 5000
                }

                Connections {
                    target: root.model

                    function onXrunsChanged() {
                        if (root.model.xruns !== 0)
                            xrunFlash.restart()
                        else {
                            xrunFlash.stop()
                            xrunsLabel.color = "#00ff00"
                        }
                    }
                }
            }

            CpuLoadMeter {
                id: cpuLoadMeter

                anchors.left: parent.left
                anchors.right: tempLabel.left
                anchors.top: audioLoadLabel.bottom
                anchors.bottom: parent.bottom
                anchors.margins: 2

                model: root.model.cpuLoad
            }

            Label {
                id: tempLabel

                width: 32

                anchors.right: parent.right
                anchors.top: xrunsLabel.bottom
                anchors.bottom: parent.bottom

                text: root.model.cpuTemp + "°C"
                font.pixelSize: 10
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                padding: 2
            }
        }
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }

    Binding {
        when: root.model
        target: Info
        property: "logMessage"
        value: root.model.logMessage
    }

    Connections {
        target: root.model

        function onMidiLearnChanged() {
            if (!root.model.midiLearn)
                MidiLearn.stop()
        }
    }
}
