// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13

import "../SettingsControls"

Item {
    id: root

    // model:
    //   .inputDevices : StringList
    //   .outputDevices : StringList
    //   .samplerates : StringList
    //   .periodSizes : StringList
    //   selectInputDevice(index)
    //   selectOutputDevice(index)
    //   selectSamplerate(index)
    //   selectPeriodSize(index)
    //   refreshDeviceLists()
    property var model

    implicitWidth: 752
    implicitHeight: 432

    ComboBoxSetting {
        id: inputSetting

        height: 64

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

        height: 64

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
        id: samplerateSetting

        height: 64

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: outputSetting.bottom
        anchors.topMargin: 6

        model: root.model.samplerates.elements
        currentIndex: root.model.samplerates.focused

        nameLabelText: qsTr("Samplerate:")
        unselectedText: qsTr("Select samplerate...")

        onOptionSelected: root.model.selectSamplerate(index)
    }

    ComboBoxSetting {
        id: periodSizeSetting

        height: 64

        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: samplerateSetting.bottom
        anchors.topMargin: 6

        model: root.model.periodSizes.elements
        currentIndex: root.model.periodSizes.focused

        nameLabelText: qsTr("Period Size:")
        unselectedText: qsTr("Select period size...")

        onOptionSelected: root.model.selectPeriodSize(index)
    }

    Frame {
        id: reloadButtonFrame

        x: 658
        width: 134
        height: 134

        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.topMargin: 8

        RoundButton {
            id: reloadBtn

            x: 19
            y: 19
            width: 72
            height: 72

            icon.source: "qrc:///images/icons/ic_refresh_white_48dp.png"
            display: AbstractButton.IconOnly

            onClicked: root.model.refreshDeviceLists()
        }
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
