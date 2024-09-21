// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import PieJam.Models 1.0

import ".."
import "../Util/MathExt.js" as MathExt

SubscribableItem {
    id: root

    QtObject {
        id: private_

        readonly property var paramModel: root.model && root.model.type === Parameter.Type.Float ? root.model : null
        readonly property real value: private_.paramModel ? MathExt.fromNormalized(private_.paramModel.normalizedValue, -1, 1) : 0
    }

    SymmetricBipolarSlider {
        anchors.fill: parent

        value: private_.value

        onMoved: {
            if (private_.paramModel) {
                private_.paramModel.changeNormalizedValue(MathExt.toNormalized(newValue, -1, 1))
                Info.showParameterValue(private_.paramModel)
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
