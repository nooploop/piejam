// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

import ".."
import "../SettingsControls"

SubscribableItem {
    id: root

    implicitWidth: 752
    implicitHeight: 432

    ComboBoxSetting {
        id: inputSetting

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: reloadButtonFrame.left
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8

        model: root.model.inputDevices.elements
        currentIndex: root.model.inputDevices.focused

        nameLabelText: qsTr("Input:")
        unselectedText: qsTr("Select input device...")

        onOptionSelected: root.model.selectInputDevice(index)
    }

    ComboBoxSetting {
        id: outputSetting

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: reloadButtonFrame.left
        anchors.rightMargin: 8
        anchors.top: inputSetting.bottom
        anchors.topMargin: 6

        model: root.model.outputDevices.elements
        currentIndex: root.model.outputDevices.focused

        nameLabelText: qsTr("Output:")
        unselectedText: qsTr("Select output device...")

        onOptionSelected: root.model.selectOutputDevice(index)
    }

    ComboBoxSetting {
        id: sampleRateSetting

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: outputSetting.bottom
        anchors.topMargin: 6

        model: root.model.sampleRates.elements
        currentIndex: root.model.sampleRates.focused

        nameLabelText: qsTr("Sample rate:")
        unselectedText: qsTr("Select sample rate...")

        onOptionSelected: root.model.selectSamplerate(index)
    }

    ComboBoxSetting {
        id: periodSizeSetting

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: sampleRateSetting.bottom
        anchors.topMargin: 6

        model: root.model.periodSizes.elements
        currentIndex: root.model.periodSizes.focused

        nameLabelText: qsTr("Period size:")
        unselectedText: qsTr("Select period size...")

        onOptionSelected: root.model.selectPeriodSize(index)
    }

    ComboBoxSetting {
        id: periodCountSetting

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: periodSizeSetting.bottom
        anchors.topMargin: 6

        model: root.model.periodCounts.elements
        currentIndex: root.model.periodCounts.focused

        nameLabelText: qsTr("Period count:")
        unselectedText: qsTr("Select period count...")

        onOptionSelected: root.model.selectPeriodCount(index)
    }

    Frame {
        id: reloadButtonFrame

        width: reloadButtonFrame.height

        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.bottom: outputSetting.bottom
        anchors.topMargin: 8

        RoundButton {
            id: reloadBtn

            width: 72
            height: 72

            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/autorenew.svg"
            display: AbstractButton.IconOnly

            onClicked: root.model.refreshDeviceLists()
        }
    }

    Frame {
        id: bufferLatencyFrame

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: periodCountSetting.bottom
        anchors.topMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8

        Label {
            id: bufferLatencyLabel

            width: 128

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left

            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            font.pixelSize: 18

            text: qsTr("Buffer latency:")
        }

        Label {
            id: bufferLatencyValueLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: bufferLatencyLabel.right
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 8

            verticalAlignment: Text.AlignVCenter
            font.italic: true
            textFormat: Text.PlainText
            font.pixelSize: 18

            text: root.model.bufferLatency.toFixed(2) + qsTr(" ms")
        }
    }
}
