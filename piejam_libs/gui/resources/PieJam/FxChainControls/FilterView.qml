// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    Column {
        id: filterTypeButtons

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        EnumButtonGroup {
            paramModel: root.content ? root.content.filterType : null
            buttonImplicitHeight: 44
        }
    }

    MidiAssignArea {
        id: midiAssign

        anchors.fill: filterTypeButtons

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
        anchors.topMargin: 4
        anchors.bottom: parent.bottom
        anchors.left: filterTypeButtons.right
        anchors.leftMargin: 8
        anchors.right: parameters.left
        anchors.rightMargin: 8

        spectrumAData: root.content ? root.content.dataIn : null
        spectrumAColor: Material.color(Material.Pink)

        spectrumBData: root.content ? root.content.dataOut : null
        spectrumBColor: Material.color(Material.Blue)

        // HACK: we reuse the level labels positions for resonance percentage labels
        // Since levels go from 0 to -80 in 20dB steps, this comes extremely handy
        // to split up the resonance percentage axis in 20% steps.
        Repeater {
            model: spectrum.levelLabels

            delegate: Label {
                id: resLabel

                x: 1
                y: modelData.position

                font.pointSize: 5

                text: (100 + modelData.value) + "%"
            }
        }

        Repeater {
            model: spectrum.levelLabels

            delegate: Label {
                id: levelLabel

                x: spectrum.width - levelLabel.implicitWidth - 4
                y: modelData.position

                font.pointSize: 5

                text: modelData.value + " dB"
            }
        }

        Repeater {
            model: spectrum.frequencyLabels

            delegate: Label {
                x: modelData.position + 2
                y: spectrum.height - implicitHeight

                font.pointSize: 5

                text: modelData.value >= 1000 ? (modelData.value / 1000) + " kHz" : modelData.value + " Hz"
            }
        }

        XYPad {
            anchors.fill: parent

            posX: root.content ? root.content.cutoff.normalizedValue : 0
            posY: root.content ? 1 - root.content.resonance.normalizedValue : 0

            handleRadius: 6
            handleColor: Material.color(Material.Yellow, Material.Shade400)

            onChangePos: {
                if (root.content)
                    root.content.cutoff.changeNormalizedValue(x)

                if (root.content)
                    root.content.resonance.changeNormalizedValue(1 - y)
            }
        }
    }

    onBypassedChanged: if (root.bypassed && root.content) root.content.clear()

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }
}
