// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0

import ".."
import "../Controls"

SubscribableItem {
    id: root

    property int orientation: Qt.Vertical
    property bool stepButtonsVisible: true

    QtObject {
        id: private_

        readonly property var paramModel: root.model && root.model.type === Parameter.Type.Int  ? root.model : null
        readonly property int from: private_.paramModel ? private_.paramModel.minValue : 0
        readonly property int to: private_.paramModel ? private_.paramModel.maxValue : 1
        readonly property int value: private_.paramModel ? private_.paramModel.value : 0

        function changeValue(newValue) {
            if (private_.paramModel) {
                private_.paramModel.changeValue(newValue)
                Info.showParameterValue(private_.paramModel)
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Button {
            display: AbstractButton.IconOnly
            icon.source: "qrc:///images/icons/chevron-up.svg"

            Layout.maximumHeight: 40
            Layout.fillWidth: true

            onClicked: private_.changeValue(Math.min(private_.value + 1, private_.to))

            visible: root.stepButtonsVisible
        }

        Slider {
            from: private_.from
            to: private_.to
            value: private_.value

            stepSize: 1
            orientation: root.orientation

            Layout.fillWidth: true
            Layout.fillHeight: true

            onMoved: private_.changeValue(value)
        }

        Button {
            display: AbstractButton.IconOnly
            icon.source: "qrc:///images/icons/chevron-down.svg"

            Layout.maximumHeight: 40
            Layout.fillWidth: true

            onClicked: private_.changeValue(Math.max(private_.value - 1, private_.from))

            visible: root.stepButtonsVisible
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
