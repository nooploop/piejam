// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import PieJam.Items 1.0 as PJItems
import PieJam.Models 1.0 as PJModels
import PieJam.FxModules.Models 1.0

import PieJam 1.0
import PieJam.FxChainControls 1.0

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    onVisibleChanged: if (root.model)
                          root.model.clear()

    onBypassedChanged: if (root.bypassed && root.model)
                           root.model.clear()

    QtObject {
        id: private_

        readonly property bool isStereo: root.model && root.model.busType === PJModels.Types.BusType.Stereo
        readonly property var mode: root.model ? root.model.mode : null
        readonly property var triggerSlope: root.model ? root.model.triggerSlope : null
        readonly property var triggerLevel: root.model ? root.model.triggerLevel : null
        readonly property var holdTime: root.model ? root.model.holdTime : null
        readonly property bool freeMode: private_.mode && private_.mode.value === FxScope.Mode.Free
        readonly property var windowSize: root.model
                                          ? (private_.freeMode ? root.model.waveformWindowSize : root.model.scopeWindowSize)
                                          : null
    }

    ColumnLayout {
        anchors.fill: parent

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: root.model ? (root.model.mode.value === FxScope.Mode.Free ? 1 : 0) : -1

            Frame {
                id: scopeView

                Layout.fillWidth: true
                Layout.fillHeight: true

                PJItems.Scope {
                    anchors.fill: parent

                    visible: root.model && root.model.activeA.value

                    scopeData: root.model ? root.model.scopeDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Scope {
                    anchors.fill: parent

                    visible: private_.isStereo && (root.model && root.model.activeB.value)

                    scopeData: root.model ? root.model.scopeDataB : null
                    color: Material.color(Material.Blue)
                }
            }

            Frame {
                id: waveformView

                Layout.fillWidth: true
                Layout.fillHeight: true

                PJItems.Waveform {
                    anchors.fill: parent

                    visible: root.model && root.model.activeA.value

                    waveformData: root.model ? root.model.waveformDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Waveform {
                    anchors.fill: parent

                    visible: private_.isStereo && (root.model && root.model.activeB.value)

                    waveformData: root.model ? root.model.waveformDataB : null
                    color: Material.color(Material.Blue)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.maximumHeight: 64

            spacing: 0

            ColumnLayout {

                Label {
                    text: "Mode"
                    topPadding: 4
                }

                EnumComboBox {
                    model: private_.mode

                    Layout.preferredWidth: 128
                    Layout.preferredHeight: 40
                }
            }

            ToolSeparator {
                Layout.fillHeight: true
            }

            ColumnLayout {
                enabled: !private_.freeMode

                Label {
                    text: "Slope"
                    topPadding: 4
                }

                EnumButtonGroup {
                    id: triggerButtons

                    model: private_.triggerSlope

                    icons: ["qrc:///images/icons/rising_edge.svg", "qrc:///images/icons/falling_edge.svg"]

                    Layout.preferredWidth: 64
                    Layout.preferredHeight: 40
                }
            }

            ToolSeparator {
                Layout.fillHeight: true
            }

            ColumnLayout {
                enabled: !private_.freeMode

                Label {
                    text: "Trigger Level"
                    topPadding: 4
                }

                ParameterQuickSpinBox {
                    model: private_.triggerLevel
                    stepScale: .5

                    Layout.preferredWidth: 112
                }
            }

            ToolSeparator {
                Layout.fillHeight: true
            }

            ColumnLayout {
                enabled: !private_.freeMode

                Label {
                    text: "Hold Time"
                    topPadding: 4
                }

                ParameterQuickSpinBox {
                    model: private_.holdTime

                    Layout.preferredWidth: 132
                }
            }

            ToolSeparator {
                Layout.fillHeight: true
            }

            ColumnLayout {
                Label {
                    text: "Window Size"
                    topPadding: 4
                }

                IntSlider {
                    model: private_.windowSize

                    orientation: Qt.Horizontal
                    stepButtonsVisible: false

                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                }
            }
        }

        ToolSeparator {
            orientation: Qt.Horizontal

            bottomPadding: 2
            topPadding: 8

            Layout.fillWidth: true
        }

        StreamSourceSettings {
            model: root.model

            Layout.fillWidth: true
            Layout.preferredHeight: 48
        }
   }

    Binding {
        when: root.model
        target: root.model
        property: "viewSize"
        value: root.model && root.model.mode.value === FxScope.Mode.Free
               ? waveformView.availableWidth
               : scopeView.availableWidth
        restoreMode: Binding.RestoreBinding
    }
}
