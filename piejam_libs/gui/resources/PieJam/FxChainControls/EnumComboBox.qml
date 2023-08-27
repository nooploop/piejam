// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

import PieJam.Models 1.0

import ".."
import "../Controls"

Item {
    id: root

    property var paramModel: null

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Enum ? root.paramModel : null
    }

    ComboBox {
        id: comboBox

        anchors.fill: parent;

        model: private_.paramModel ? private_.paramModel.values : null
        currentIndex: private_.paramModel ? private_.paramModel.value - private_.paramModel.minValue : -1
        textRole: "text"
        valueRole: "value"

        onActivated: {
            if (private_.paramModel) {
                private_.paramModel.changeValue(private_.paramModel.minValue + index)
                Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + private_.paramModel.valueString
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
