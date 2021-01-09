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

    ComboBoxSetting {
        id: inputSetting

        x: 8
        y: 8
        width: 644
        height: 64

        model: root.model.inputDevices.elements
        currentIndex: root.model.inputDevices.focused

        nameLabelText: qsTr("Input:")

        onOptionSelected: root.model.selectInputDevice(index)
    }

    ComboBoxSetting {
        id: outputSetting

        x: 8
        y: 78
        width: 644
        height: 64

        model: root.model.outputDevices.elements
        currentIndex: root.model.outputDevices.focused

        nameLabelText: qsTr("Output:")

        onOptionSelected: root.model.selectOutputDevice(index)
    }

    ComboBoxSetting {
        id: samplerateSetting

        x: 8
        y: 148
        width: 784
        height: 64

        model: root.model.samplerates.elements
        currentIndex: root.model.samplerates.focused

        nameLabelText: qsTr("Samplerate:")

        onOptionSelected: root.model.selectSamplerate(index)
    }

    Frame {
        id: periodSizeFrame

        x: 8
        y: 218
        width: 784
        height: 110

        Label {
            id: periodSizeLabel
            x: 0
            y: 0
            width: 120
            height: 40
            text: qsTr("Period Size:")
            font.pixelSize: 18
            verticalAlignment: Text.AlignVCenter
        }

        Slider {
            id: periodSizeSlider

            visible: root.model && root.model.periodSizes.elements.length > 0

            x: 126
            y: 0
            width: 634
            height: 40

            stepSize: 1
            from: 0
            to: root.model.periodSizes.elements.length - 1
            value: root.model.periodSizes.focused

            onMoved: root.model.selectPeriodSize(periodSizeSlider.value)
        }

        Label {
            id: periodSize

            visible: root.model && root.model.periodSizes.elements.length > 0

            x: 126
            y: 46
            width: 634
            height: 40

            text: root.model.periodSizes.elements[root.model.periodSizes.focused] + qsTr(" Samples")
            leftPadding: 6
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
        }

        Label {
            id: unavailableLabel

            visible: !root.model || root.model.periodSizes.elements.length === 0

            x: periodSizeSlider.x
            y: periodSizeSlider.y
            width: periodSizeSlider.width
            height: periodSizeSlider.height

            text: "Unavailable"
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 18
        }
    }

    Frame {
        x: 658
        y: 8
        width: 134
        height: 134

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
