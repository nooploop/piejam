// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels

import "../Controls"

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    Column {
        id: typeButtons

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        EnumButtonGroup {
            id: filterButtons

            paramModel: root.content ? root.content.filterType : null
            buttonHeight: 46
        }
    }

    MidiAssignArea {
        id: midiAssign

        anchors.fill: typeButtons

        model: root.content ? root.content.filterType.midi : null
    }

    Row {
        id: parameters

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        spacing: 4

        ParameterControl {
            id: freqSlider

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            paramModel: root.content ? root.content.cutoff : null
        }

        ParameterControl {
            id: resSlider

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            paramModel: root.content ? root.content.resonance : null
        }
    }

    PJItems.Spectrum {
        id: spectrum

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: typeButtons.right
        anchors.leftMargin: 8
        anchors.right: parameters.left
        anchors.rightMargin: 8

        spectrumAData: root.content ? root.content.dataIn : null
        spectrumAColor: Material.color(Material.Pink)

        spectrumBData: root.content ? root.content.dataOut : null
        spectrumBColor: Material.color(Material.Blue)

        Repeater {
            model: spectrum.levelLabels

            delegate: Label {
                id: levelLabel

                x: spectrum.width - levelLabel.implicitWidth - 4
                y: modelData.position

                font.pointSize: 6

                text: modelData.value + " dB"
            }
        }

        Repeater {
            model: spectrum.frequencyLabels

            delegate: Label {
                x: modelData.position + 2
                y: spectrum.height - implicitHeight

                font.pointSize: 6

                text: modelData.value >= 1000 ? (modelData.value / 1000) + " kHz" : modelData.value + " Hz"
            }
        }

        XYPad {
            anchors.fill: parent

            posX: root.content ? root.content.cutoff.value : 0
            posY: root.content ? 1 - root.content.resonance.value : 0

            onPosChanged: {
                if (root.content)
                    root.content.cutoff.changeValue(x)

                if (root.content)
                    root.content.resonance.changeValue(1 - y)
            }
        }
    }

    onBypassedChanged: if (root.bypassed && root.content) root.content.clear()


    Binding {
        when: root.content
        target: root.content
        property: "subscribed"
        value: root.visible
        restoreMode: Binding.RestoreBinding
    }
}
