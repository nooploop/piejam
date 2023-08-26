// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import "../Controls"

Item {
    id: root

    property var paramModel: null
    property real stepScale: 1

    implicitWidth: 96
    implicitHeight: 40

    QuickSpinBox {
        from: 0
        to: 10000
        step: 100 * root.stepScale
        bigStep: 500 * root.stepScale
        value: paramModel ? paramModel.normalizedValue * 10000 : 0

        textFromValue: function(value) {
            return paramModel ? paramModel.valueString : "#"
        }

        anchors.fill: parent

        onChangeValue: {
            if (paramModel)
                paramModel.changeNormalizedValue(newValue / 10000)
        }

        onReset: {
            if (paramModel)
                paramModel.resetToDefault()
        }
    }
}
