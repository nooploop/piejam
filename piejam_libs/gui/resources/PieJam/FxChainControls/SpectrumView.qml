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

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    QtObject {
        id: private_

        readonly property bool isStereo: root.content && root.content.busType == PJModels.BusType.Stereo
    }

    ColumnLayout {
        anchors.fill: parent

        PJItems.Spectrum {
            id: spectrum

            Layout.fillWidth: true
            Layout.fillHeight: true

            spectrumAData: root.content ? root.content.dataA : null
            spectrumAColor: Material.color(Material.Pink)

            spectrumBData: root.content ? root.content.dataB : null
            spectrumBColor: Material.color(Material.Blue)
        }

        RowLayout {
            Layout.fillWidth: true

            StereoChannelSelector {
                visible: private_.isStereo

                name: "A"
                activeParam: root.content ? root.content.activeA : null
                channelParam: root.content ? root.content.channelA : null
                gainParam: root.content ? root.content.gainA : null

                Material.accent: root.content && root.content.activeA.value ? Material.Pink : Material.Grey

                Layout.fillWidth: true
            }

            ToolSeparator {
                visible: private_.isStereo
            }

            StereoChannelSelector {
                visible: private_.isStereo

                name: "B"
                activeParam: root.content ? root.content.activeB : null
                channelParam: root.content ? root.content.channelB : null
                gainParam: root.content ? root.content.gainB : null

                Material.accent: root.content && root.content.activeB.value ? Material.Blue : Material.Grey

                Layout.fillWidth: true
            }

            ParameterQuickSpinBox {
                visible: !private_.isStereo

                paramModel: root.content ? root.content.gainA : null
                stepScale: 1.04167

                Layout.preferredWidth: 128
            }

            Item {
                visible: !private_.isStereo

                Layout.fillWidth: true
            }

            ToolSeparator {}

            ParameterToggleButton {
                paramModel: root.content ? root.content.freeze : null

                Layout.preferredWidth: 32
                Layout.preferredHeight: 40

                flat: false

                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icons/snow.svg"
            }
        }
    }

    onBypassedChanged: if (root.bypassed && root.content)
                           root.content.clear()

    Repeater {
        model: spectrum.levelLabels

        delegate: Label {
            x: spectrum.width - implicitWidth - 4
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

            text: modelData.value >= 1000 ? (modelData.value / 1000)
                                            + " kHz" : modelData.value + " Hz"
        }
    }

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }
}
