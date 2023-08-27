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
    property var triggerSource: null
    property var triggerSlope: null
    property var triggerLevel: null
    property var holdTime: null

    implicitHeight: 64

    QtObject {
        id: private_

        readonly property bool freeMode: root.triggerSource && root.triggerSource.value === PJModels.FxScope.TriggerSource.Free
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {

            Label {
                text: "Mode"
                topPadding: 4
            }

            EnumComboBox {
                paramModel: root.triggerSource

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

                paramModel: root.triggerSlope

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
                paramModel: root.triggerLevel
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
                paramModel: root.holdTime

                Layout.preferredWidth: 132
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
