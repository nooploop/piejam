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

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    EnumButtonGroup {
        id: filterTypeButtons

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        width: 64

        alignment: Qt.Vertical
        spacing: 0

        model: root.model ? root.model.filterType : null
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

            paramModel: root.model ? root.model.cutoff : null
        }

        ParameterControl {
            id: resSlider

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            paramModel: root.model ? root.model.resonance : null
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

        spectrumAData: root.model ? root.model.dataIn : null
        spectrumAColor: Material.color(Material.Pink)

        spectrumBData: root.model ? root.model.dataOut : null
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

            posX: root.model ? root.model.cutoff.normalizedValue : 0
            posY: root.model ? 1 - root.model.resonance.normalizedValue : 0

            handleRadius: 6
            handleColor: Material.color(Material.Yellow, Material.Shade400)

            onChangePos: {
                if (root.model)
                    root.model.cutoff.changeNormalizedValue(x)

                if (root.model)
                    root.model.resonance.changeNormalizedValue(1 - y)
            }
        }
    }

    onBypassedChanged: if (root.bypassed && root.model) root.model.clear()
}
