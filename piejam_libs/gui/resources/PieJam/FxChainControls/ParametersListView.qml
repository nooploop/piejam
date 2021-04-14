// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property var content: null

    implicitWidth: listView.contentWidth

    ListView {
        id: listView

        width: contentWidth

        orientation: Qt.Horizontal

        spacing: 4

        model: if (root.content) root.content.parameters

        delegate: ParameterControl {
            name: model.item.name
            value: model.item.value
            valueText: model.item.valueString
            switchValue: model.item.switchValue

            sliderMin: model.item.stepped ? model.item.minValue : 0.0
            sliderMax: model.item.stepped ? model.item.maxValue : 1.0
            sliderStep: model.item.stepped ? 1.0 : 0.0

            isSwitch: model.item.isSwitch

            midi.model: model.item.midi

            height: root.height

            onSliderMoved: model.item.changeValue(newValue)
            onSwitchToggled: model.item.changeSwitchValue(newValue)

            Binding {
                target: model.item
                property: "subscribed"
                value: visible
            }
        }
    }
}
