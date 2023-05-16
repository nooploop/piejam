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

import ".."

Item {
    id: root

    property var content: null
    property bool bypassed: false

    implicitWidth: 636

    onVisibleChanged: if (root.content)
                          root.content.clear()

    onBypassedChanged: if (root.bypassed && root.content)
                           root.content.clear()

    ColumnLayout {
        anchors.fill: parent

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            PJItems.Scope {
                id: scopeA

                anchors.fill: parent

                lines: root.content ? root.content.dataA : null
                color: Material.color(Material.Pink)
            }

            PJItems.Scope {
                id: scopeB

                anchors.fill: parent

                lines: root.content ? root.content.dataB : null
                color: Material.color(Material.Blue)
            }
        }

        Slider {
            id: resolutionSlider

            Layout.fillWidth: true

            from: 0
            to: 11
            stepSize: 1
            value: 8
        }

        RowLayout {
            Layout.fillWidth: true

            visible: root.content
                     && root.content.busType == PJModels.BusType.Stereo

            StereoChannelSelector {
                id: channelASelector

                name: "A"
                active: root.content ? root.content.activeA : false
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
                active: root.content ? root.content.activeB : false
                channel: root.content ? root.content.channelB : PJModels.StereoChannel.Right

                Material.accent: Material.Blue

                onActiveToggled: root.content.changeActiveB(!active)
                onChannelSelected: root.content.changeChannelB(ch)
            }
        }
    }

    Binding {
        when: root.content
        target: root.content
        property: "viewSize"
        value: scopeA.width
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "samplesPerLine"
        value: Math.pow(2, resolutionSlider.value)
        restoreMode: Binding.RestoreBinding
    }

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }
}
