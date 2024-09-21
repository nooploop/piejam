// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels

import PieJam 1.0
import PieJam.FxChainControls 1.0

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    RowLayout {
        anchors.fill: parent

        EnumButtonGroup {
            Layout.preferredWidth: 64
            Layout.fillHeight: true

            alignment: Qt.Vertical
            spacing: 0

            model: root.model ? root.model.filterType : null
        }

        PJItems.Spectrum {
            id: spectrum

            Layout.fillWidth: true
            Layout.fillHeight: true

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

            ParameterXYPad {
                anchors.fill: parent

                modelX: root.model ? root.model.cutoff : null
                modelY: root.model ? root.model.resonance : null
            }
        }

        ParameterControl {
            Layout.fillHeight: true

            paramModel: root.model ? root.model.cutoff : null
        }

        ParameterControl {
            Layout.fillHeight: true

            paramModel: root.model ? root.model.resonance : null
        }
    }

    onBypassedChanged: if (root.bypassed && root.model) root.model.clear()
}
