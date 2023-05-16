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

            visible: root.content
                     && root.content.busType == PJModels.BusType.Stereo

            StereoChannelSelector {
                id: channelASelector

                name: "A"
                active: root.content && root.content.activeA
                channel: root.content ? root.content.channelA : PJModels.StereoChannel.Left

                Material.accent: Material.Pink

                onActiveToggled: root.content.changeActiveA(!active)
                onChannelSelected: root.content.changeChannelA(ch)
            }

            Item {
                Layout.fillWidth: true
            }

            StereoChannelSelector {
                id: channelBSelector

                name: "B"
                active: root.content && root.content.activeB
                channel: root.content ? root.content.channelB : PJModels.StereoChannel.Right

                Material.accent: Material.Blue

                onActiveToggled: root.content.changeActiveB(!active)
                onChannelSelected: root.content.changeChannelB(ch)
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
