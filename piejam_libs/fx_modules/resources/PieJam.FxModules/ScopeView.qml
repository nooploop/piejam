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
import PieJam.Util 1.0

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
        readonly property bool activeA: root.model && root.model.activeA.value
        readonly property bool activeB: root.model && root.model.activeB.value
        readonly property bool visibleA: !isStereo || activeA
        readonly property bool visibleB: isStereo && activeB
        readonly property var mode: root.model ? root.model.mode : null
        readonly property var triggerSlope: root.model ? root.model.triggerSlope : null
        readonly property var triggerLevel: root.model ? root.model.triggerLevel : null
        readonly property var holdTime: root.model ? root.model.holdTime : null
        readonly property bool freeMode: mode && mode.value === FxScope.Mode.Free
        readonly property bool scopeWindowSize: root.model ? root.model.scopeWindowSize.value : 0
        readonly property var windowSize: root.model
                                          ? (freeMode ? root.model.waveformWindowSize : root.model.scopeWindowSize)
                                          : null
    }

    ColumnLayout {
        anchors.fill: parent

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: root.model ? private_.freeMode : -1

            Frame {
                id: scopeView

                Layout.fillWidth: true
                Layout.fillHeight: true

                property real syncedPeakLevel: scopeA.visible ? (scopeB.visible ? Math.max(scopeA.peakLevel, scopeB.peakLevel) : scopeA.peakLevel)
                                                              : (scopeB.visible ? scopeB.peakLevel : 0)

                onSyncedPeakLevelChanged: {
                    if (scopeA.visible) scopeA.syncPeakLevel(syncedPeakLevel)
                    if (scopeB.visible) scopeB.syncPeakLevel(syncedPeakLevel)
                }

                PJItems.FixedLinearScaleGrid {
                    anchors.fill: parent

                    ticks: [1.0, 0.0, -1.0]
                    orientation: Qt.Vertical
                    color: Material.frameColor

                    PJItems.FixedLinearScaleGrid {
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom

                        ticks: [0.0, 1.0, 5.0, 10.0]
                        orientation: Qt.Horizontal
                        color: Material.frameColor

                        height: 4
                        width: root.model ? root.model.sampleRate / 100 : parent.width
                    }

                    Repeater {
                        model: [1.0, 5.0, 10.0]
                        delegate: Label {
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 4

                            visible: root.model
                            x: (root.model ? modelData * (root.model.sampleRate / 1000) : 0) - 4

                            text: modelData * (root.model ? (root.model.scopeWindowSize.value * 2 + 1) : 1) + " ms"
                            color: Material.frameColor
                            font.pixelSize: 12
                        }
                    }
                }

                PJItems.Scope {
                    id: scopeA

                    anchors.fill: parent

                    visible: private_.visibleA

                    scope: root.model ? root.model.scopeA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Scope {
                    id: scopeB

                    anchors.fill: parent

                    visible: private_.visibleB

                    scope: root.model ? root.model.scopeB : null
                    color: Material.color(Material.Blue)
                }

                Label {
                    anchors.left: parent.left
                    anchors.top: parent.top

                    text: "Level: " + (scopeView.syncedPeakLevel === 0 ? "-inf dB" : (DbConvert.to_dB(scopeView.syncedPeakLevel).toFixed(1) + " dB"))
                }
            }

            Frame {
                id: waveformView

                Layout.fillWidth: true
                Layout.fillHeight: true

                PJItems.FixedLinearScaleGrid {
                    anchors.fill: parent

                    ticks: [1.0, 0.0, -1.0]
                    orientation: Qt.Vertical
                    color: Material.frameColor
                }

                PJItems.Waveform {
                    anchors.fill: parent

                    visible: private_.visibleA

                    waveform: root.model ? root.model.waveformA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Waveform {
                    anchors.fill: parent

                    visible: private_.visibleB

                    waveform: root.model ? root.model.waveformB : null
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

                    Layout.preferredWidth: 148
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
        value: private_.freeMode ? waveformView.availableWidth : scopeView.availableWidth
        restoreMode: Binding.RestoreBinding
    }
}
