// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0

import ".."

Item {
    id: root

    property var paramModel: null

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Int  ? root.paramModel : null
        readonly property int from: private_.paramModel ? private_.paramModel.minValue : 0
        readonly property int to: private_.paramModel ? private_.paramModel.maxValue : 1
        readonly property int value: private_.paramModel ? private_.paramModel.value : 0

        function changeValue(newValue) {
            if (private_.paramModel) {
                private_.paramModel.changeValue(newValue)
                Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + private_.paramModel.valueString
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
        }

        Slider {
            from: private_.from
            to: private_.to
            value: private_.value

            stepSize: 1
            orientation: Qt.Vertical

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
        }
    }
}
