// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import ".."
import "../SettingsControls"

SubscribableItem {
    id: root

    implicitWidth: 752
    implicitHeight: 432

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                Layout.fillWidth: true

                ComboBoxSetting {
                    Layout.fillWidth: true

                    model: root.model.inputSoundCards.elements
                    currentIndex: root.model.inputSoundCards.focused

                    nameLabelText: qsTr("input:")
                    unselectedText: qsTr("Select input...")

                    onOptionSelected: root.model.selectInputSoundCard(index)
                }

                ComboBoxSetting {
                    Layout.fillWidth: true

                    model: root.model.outputSoundCards.elements
                    currentIndex: root.model.outputSoundCards.focused

                    nameLabelText: qsTr("output:")
                    unselectedText: qsTr("Select output...")

                    onOptionSelected: root.model.selectOutputSoundCard(index)
                }
            }

            Frame {
                Layout.preferredWidth: 117
                Layout.preferredHeight: 117

                RoundButton {
                    id: reloadBtn

                    width: 72
                    height: 72

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/cycle_arrows.svg"
                    display: AbstractButton.IconOnly

                    onClicked: root.model.refreshSoundCardLists()
                }
            }
        }

        ComboBoxSetting {
            Layout.fillWidth: true

            model: root.model.sampleRates.elements
            currentIndex: root.model.sampleRates.focused

            nameLabelText: qsTr("sample rate:")
            unselectedText: qsTr("Select sample rate...")

            onOptionSelected: root.model.selectSampleRate(index)
        }

        ComboBoxSetting {
            Layout.fillWidth: true

            model: root.model.periodSizes.elements
            currentIndex: root.model.periodSizes.focused

            nameLabelText: qsTr("period size:")
            unselectedText: qsTr("Select period size...")

            onOptionSelected: root.model.selectPeriodSize(index)
        }

        ComboBoxSetting {
            Layout.fillWidth: true

            model: root.model.periodCounts.elements
            currentIndex: root.model.periodCounts.focused

            nameLabelText: qsTr("period count:")
            unselectedText: qsTr("Select period count...")

            onOptionSelected: root.model.selectPeriodCount(index)
        }

        Frame {
            Layout.fillWidth: true
            Layout.preferredHeight: 64

            RowLayout {
                anchors.fill: parent

                Label {
                    Layout.preferredWidth: 128
                    Layout.fillHeight: true

                    verticalAlignment: Text.AlignVCenter
                    textFormat: Text.PlainText
                    font.pixelSize: 18

                    text: qsTr("buffer latency:")
                }

                Label {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    verticalAlignment: Text.AlignVCenter
                    font.italic: true
                    textFormat: Text.PlainText
                    font.pixelSize: 18

                    text: root.model.bufferLatency.toFixed(2) + qsTr(" ms")
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
