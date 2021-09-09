// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."
import "../Controls"

Item {
    id: root

    property var paramModel: null

    signal sliderMoved(real newValue)
    signal switchToggled(bool newValue)

    implicitWidth: 82

    Rectangle {
        id: headerRect

        anchors.top: parent.top
        anchors.right: parent.right

        width: 82
        height: 70

        color: Material.color(Material.Grey, Material.Shade800)
        radius: 2

        HeaderLabel {
            id: nameLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            height: 40

            text: root.paramModel ? root.paramModel.name : "name"

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            wrapMode: Text.WordWrap

            backgroundColor: Material.color(Material.Grey, Material.Shade800)
        }

        Label {
            id: valueLabel

            text: root.paramModel ? root.paramModel.valueString : "value"

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 4

            padding: 2
            background: Rectangle {
                color: Material.color(Material.Grey, Material.Shade700)
                radius: 2
            }

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            font.pixelSize: 12
        }
    }

    Rectangle {
        id: sliderRect

        anchors.top: headerRect.bottom
        anchors.topMargin: 4
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        color: Material.color(Material.Grey, Material.Shade800)
        radius: 2

        StackLayout {
            id: controlStack

            anchors.fill: parent
            anchors.margins: 8

            currentIndex: root.paramModel ? (root.paramModel.isSwitch ? 1 : 0) : 0

            Slider {
                id: valueSlider

                from: root.paramModel ? (root.paramModel.stepped ? root.paramModel.minValue : 0.0) : 0.0
                to: root.paramModel ? (root.paramModel.stepped ? root.paramModel.maxValue : 1.0) : 1.0
                stepSize: root.paramModel ? (root.paramModel.stepped ? 1.0 : 0.0) : 0.0

                value: root.paramModel ? root.paramModel.value : 0

                orientation: Qt.Vertical

                onMoved: {
                    if (root.paramModel)
                        root.paramModel.changeValue(valueSlider.value)
                    sliderMoved(valueSlider.value)
                    Info.quickTip = "<b>" + nameLabel.text + "</b>: " + valueLabel.text
                }
            }

            Switch {
                id: toggleSwitch

                checked: root.paramModel ? root.paramModel.switchValue : false

                onToggled: {
                    if (root.paramModel)
                        root.paramModel.changeSwitchValue(toggleSwitch.checked)
                    root.switchToggled(toggleSwitch.checked)
                    Info.quickTip = "<b>" + nameLabel.text + "</b>: " + (root.switchValue ? "on" : "off")
                }
            }
        }

        MidiAssignArea {
            id: midiAssign

            anchors.fill: controlStack

            model: root.paramModel ? root.paramModel.midi : null
        }
    }
}
