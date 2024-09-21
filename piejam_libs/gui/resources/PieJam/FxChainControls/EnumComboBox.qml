// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

import PieJam.Models 1.0

import ".."
import "../Controls"

SubscribableItem {
    id: root

    QtObject {
        id: private_

        readonly property var paramModel: root.model && root.model.type === Parameter.Type.Enum ? root.model : null
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
                Info.showParameterValue(private_.paramModel)
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
