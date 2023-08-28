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

    QtObject {
        id: private_

        readonly property bool isStereo: root.content && root.content.busType == PJModels.BusType.Stereo
    }

    ColumnLayout {

        anchors.fill: parent

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: root.content ? (root.content.mode.value === PJModels.FxScope.Mode.Free ? 1 : 0) : -1

            Frame {
                id: scopeView

                Layout.fillWidth: true
                Layout.fillHeight: true

                PJItems.Scope {
                    id: scopeA

                    anchors.fill: parent

                    scopeData: root.content ? root.content.scopeDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Scope {
                    id: scopeB

                    anchors.fill: parent

                    visible: private_.isStereo && (root.content && root.content.activeB.value)

                    scopeData: root.content ? root.content.scopeDataB : null
                    color: Material.color(Material.Blue)
                }
            }

            Frame {
                id: waveformView

                Layout.fillWidth: true
                Layout.fillHeight: true

                PJItems.Waveform {
                    id: waveformA

                    anchors.fill: parent

                    waveformData: root.content ? root.content.waveformDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Waveform {
                    id: waveformB

                    anchors.fill: parent

                    visible: private_.isStereo && (root.content && root.content.activeB.value)

                    waveformData: root.content ? root.content.waveformDataB : null
                    color: Material.color(Material.Blue)
                }
            }

        }

        ScopeSettings {
            model: root.content

            Layout.fillWidth: true
        }

        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }

        RowLayout {

            Layout.fillWidth: true
            Layout.preferredHeight: 40

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

            Button {
                id: freezeButton

                icon.width: 24
                icon.height: 24
                icon.source: "qrc:///images/icons/snow.svg"

                implicitWidth: 32
                implicitHeight: 40

                checkable: true
                checked: root.content && root.content.freeze

                Binding {
                    when: root.content
                    target: root.content
                    property: "freeze"
                    value: freezeButton.checked
                    restoreMode: Binding.RestoreBinding
                }
            }
        }
    }

    Binding {
        when: root.content
        target: root.content
        property: "viewSize"
        value: root.content && root.content.mode.value === PJModels.FxScope.Mode.Free ? waveformA.width : scopeA.width
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "freeze"
        value: freezeButton.checked
        restoreMode: Binding.RestoreBinding
    }

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }
}
