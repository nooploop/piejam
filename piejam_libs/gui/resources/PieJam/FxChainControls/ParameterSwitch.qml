// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Models 1.0

import ".."

Item {
    id: root

    property var paramModel: null

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Bool ? root.paramModel : null
    }

    Switch {
        checked: private_.paramModel && private_.paramModel.value

        onToggled: {
            if (private_.paramModel) {
                private_.paramModel.changeValue(toggleSwitch.checked)
                Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + (private_.paramModel.value ? "on" : "off")
            }
        }
    }
}
