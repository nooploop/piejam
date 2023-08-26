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
        id: privates

        readonly property bool isStereo: root.content && root.content.busType == PJModels.BusType.Stereo
    }

    ColumnLayout {

        anchors.fill: parent

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: root.content ? (root.content.triggerSource.value === PJModels.FxScope.TriggerSource.Free ? 1 : 0) : -1

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

                    visible: privates.isStereo && (root.content ? root.content.activeB : false)

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

                    visible: privates.isStereo && (root.content ? root.content.activeB : false)

                    waveformData: root.content ? root.content.waveformDataB : null
                    color: Material.color(Material.Blue)
                }
            }

        }

        ScopeSettings {
            id: settings

            isStereo: privates.isStereo

            mode: root.content ? root.content.triggerSource : null

            triggerSlope: root.content ? root.content.triggerSlope : PJModels.TriggerSlope.RisingEdge

            Binding {
                when: root.content
                target: root.content
                property: "triggerSlope"
                value: settings.triggerSlope
                restoreMode: Binding.RestoreBinding
            }

            triggerLevel: root.content ? root.content.triggerLevel * 100 : 0

            Binding {
                when: root.content
                target: root.content
                property: "triggerLevel"
                value: settings.triggerLevel / 100
                restoreMode: Binding.RestoreBinding
            }

            holdTime: root.content ? root.content.holdTime : 80

            Binding {
                when: root.content
                target: root.content
                property: "holdTime"
                value: settings.holdTime
                restoreMode: Binding.RestoreBinding
            }

            Layout.fillWidth: true
        }

        ToolSeparator {
            orientation: Qt.Horizontal
            Layout.fillWidth: true
        }

        RowLayout {

            Layout.fillWidth: true

            StereoChannelSelector {
                id: channelASelector

                visible: privates.isStereo

                name: "A"
                active: root.content ? root.content.activeA : false
                channel: root.content ? root.content.channelA : PJModels.StereoChannel.Left

                Material.accent: Material.Pink

                onActiveToggled: root.content.changeActiveA(!active)
                onChannelSelected: root.content.changeChannelA(ch)
            }

            ToolSeparator {
                visible: privates.isStereo
            }

            StereoChannelSelector {
                id: channelBSelector

                visible: privates.isStereo

                name: "B"
                active: root.content ? root.content.activeB : false
                channel: root.content ? root.content.channelB : PJModels.StereoChannel.Right

                Material.accent: Material.Blue

                onActiveToggled: root.content.changeActiveB(!active)
                onChannelSelected: root.content.changeChannelB(ch)
            }

            ToolSeparator {
                visible: privates.isStereo
            }

            StackLayout {
                currentIndex: root.content ? (root.content.triggerSource.value === PJModels.FxScope.TriggerSource.Free ? 1 : 0) : -1

                Layout.fillWidth: true
                Layout.maximumHeight: 40

                Slider {
                    id: scopeResolutionSlider

                    Layout.fillWidth: true

                    from: 1
                    to: 8
                    stepSize: 1

                    value: root.content ? root.content.scopeResolution : 1
                    onMoved: if (root.content) root.content.scopeResolution = value
                }

                Slider {
                    id: waveformResolutionSlider

                    Layout.fillWidth: true

                    from: 0
                    to: 11
                    stepSize: 1
                    value: 8
                }
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
        value: root.content && root.content.triggerSource.value === PJModels.FxScope.TriggerSource.Free ? waveformA.width : scopeA.width
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "triggerSlope"
        value: settings.triggerSlope
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "triggerLevel"
        value: settings.triggerLevel / 100
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "holdTime"
        value: settings.holdTime
        restoreMode: Binding.RestoreBinding
    }

    Binding {
        when: root.content
        target: root.content
        property: "samplesPerPixel"
        value: Math.pow(2, waveformResolutionSlider.value)
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
