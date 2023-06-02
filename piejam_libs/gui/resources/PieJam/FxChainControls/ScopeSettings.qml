// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0 as PJModels

import "../Controls"

Item {
    id: root

    property bool isStereo: false
    property alias mode: modeComboBox.currentText
    property int triggerSlope: PJModels.TriggerSlope.RisingEdge
    property alias triggerLevel: triggerLevelSpinBox.value
    property alias holdTime: holdTimeSpinBox.value

    implicitHeight: 64

    RowLayout {
        anchors.fill: parent

        ColumnLayout {

            Label {
                text: "Mode"
                topPadding: 4
            }

            ComboBox {
                id: modeComboBox

                implicitWidth: 128
                implicitHeight: 40

                model: root.isStereo ? ["Trigger A", "Trigger B", "Free"] : ["Trigger", "Free"]
            }
        }

        ToolSeparator {
            Layout.fillHeight: true
        }

        ColumnLayout {

            enabled: modeComboBox.currentText !== "Free"

            Label {
                text: "Slope"
                topPadding: 4
            }

            ButtonGroup {
                buttons: triggerButtons.children
            }

            RowLayout {
                id: triggerButtons

                Button {
                    implicitWidth: 32
                    implicitHeight: 40

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/rising_edge.svg"

                    autoExclusive: true
                    checkable: true
                    checked: root.triggerSlope == PJModels.TriggerSlope.RisingEdge

                    onClicked: root.triggerSlope = PJModels.TriggerSlope.RisingEdge
                }

                Button {
                    implicitWidth: 32
                    implicitHeight: 40

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/falling_edge.svg"

                    autoExclusive: true
                    checkable: true
                    checked: root.triggerSlope == PJModels.TriggerSlope.FallingEdge

                    onClicked: root.triggerSlope = PJModels.TriggerSlope.FallingEdge
                }
            }
        }

        ToolSeparator {
            Layout.fillHeight: true
        }

        ColumnLayout {

            enabled: modeComboBox.currentText !== "Free"

            Label {
                text: "Trigger Level"
                topPadding: 4
            }

            QuickSpinBox {
                id: triggerLevelSpinBox

                from: -100
                to: 100
                step: 1
                bigStep: 5
                value: 0

                implicitWidth: 112

                textFromValue: function(value) {
                    return (value / 100).toFixed(2)
                }
            }
        }

        ToolSeparator {
            Layout.fillHeight: true
        }

        ColumnLayout {

            enabled: modeComboBox.currentText !== "Free"

            Label {
                text: "Hold Time"
                topPadding: 4
            }

            QuickSpinBox {
                id: holdTimeSpinBox

                from: 16
                to: 1600
                step: 16
                bigStep: 160
                value: 80

                implicitWidth: 128

                textFromValue: function(value) {
                    return value >= 1000 ? ((value / 1000).toFixed(2) + " s" )  : (value + " ms")
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
