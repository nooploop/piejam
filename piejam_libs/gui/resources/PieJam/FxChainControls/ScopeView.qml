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

        readonly property bool isStereo: root.content && root.content.busType === PJModels.Types.BusType.Stereo
        readonly property var mode: root.content ? root.content.mode : null
        readonly property var triggerSlope: root.content ? root.content.triggerSlope : null
        readonly property var triggerLevel: root.content ? root.content.triggerLevel : null
        readonly property var holdTime: root.content ? root.content.holdTime : null
        readonly property bool freeMode: private_.mode && private_.mode.value === PJModels.FxScope.Mode.Free
        readonly property var windowSize: root.content
                                          ? (private_.freeMode ? root.content.waveformWindowSize : root.content.scopeWindowSize)
                                          : null
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
                    anchors.fill: parent

                    visible: root.content && root.content.activeA.value

                    scopeData: root.content ? root.content.scopeDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Scope {
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
                    anchors.fill: parent

                    visible: root.content && root.content.activeA.value

                    waveformData: root.content ? root.content.waveformDataA : null
                    color: Material.color(Material.Pink)
                }

                PJItems.Waveform {
                    anchors.fill: parent

                    visible: private_.isStereo && (root.content && root.content.activeB.value)

                    waveformData: root.content ? root.content.waveformDataB : null
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
                    paramModel: private_.mode

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

                    paramModel: private_.triggerSlope

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
                    paramModel: private_.triggerLevel
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
                    paramModel: private_.holdTime

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
                    paramModel: private_.windowSize

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
            model: root.content

            Layout.fillWidth: true
            Layout.preferredHeight: 48
        }
   }

    Binding {
        when: root.content
        target: root.content
        property: "viewSize"
        value: root.content && root.content.mode.value === PJModels.FxScope.Mode.Free
               ? waveformView.availableWidth
               : scopeView.availableWidth
        restoreMode: Binding.RestoreBinding
    }

    ModelSubscription {
        target: root.content
        subscribed: root.visible
    }
}
