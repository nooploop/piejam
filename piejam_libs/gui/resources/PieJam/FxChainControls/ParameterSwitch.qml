// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Models 1.0

import ".."
import "../Controls"

Item {
    id: root

    property var paramModel: null

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Bool ? root.paramModel : null
        readonly property bool value: private_.paramModel && private_.paramModel.value
    }

    Switch {
        anchors.fill: parent

        checked: private_.value

        onToggled: {
            if (private_.paramModel) {
                private_.paramModel.changeValue(!private_.value)
                Info.showParameterValue(private_.paramModel)
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
