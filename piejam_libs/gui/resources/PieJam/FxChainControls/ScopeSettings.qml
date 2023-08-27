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

    property var model: null

    implicitHeight: 64

    QtObject {
        id: private_

        readonly property var mode: root.model ? root.model.mode : null
        readonly property var triggerSlope: root.model ? root.model.triggerSlope : null
        readonly property var triggerLevel: root.model ? root.model.triggerLevel : null
        readonly property var holdTime: root.model ? root.model.holdTime : null
        readonly property bool freeMode: private_.mode && private_.mode.value === PJModels.FxScope.Mode.Free
        readonly property var windowSize: root.model
                                          ? (private_.freeMode ? root.model.waveformWindowSize : root.model.scopeWindowSize)
                                          : null
    }

    RowLayout {
        anchors.fill: parent

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
}
