// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

import "../Controls"

Item {
    id: root

    property alias name: nameLabel.text
    property alias valueText: valueLabel.text
    property alias value: valueSlider.value
    property alias switchValue: toggleSwitch.checked

    property alias sliderMin: valueSlider.from
    property alias sliderMax: valueSlider.to
    property alias sliderStep: valueSlider.stepSize

    property bool isSwitch: false

    signal sliderMoved(real newValue)
    signal switchToggled(bool newValue)

    implicitWidth: nameLabel.width

    HeaderLabel {
        id: nameLabel

        text: "name"

        width: sliderRect.implicitWidth

        anchors.top: parent.top
        anchors.topMargin: 4
        anchors.horizontalCenter: parent.horizontalCenter

        backgroundColor: Material.color(Material.Grey, Material.Shade800)
    }

    Rectangle {
        id: sliderRect

        anchors.top: nameLabel.bottom
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 4

        implicitWidth: valueLabel.width + 8

        color: Material.color(Material.Grey, Material.Shade800)
        radius: 2

        Label {
            id: valueLabel

            text: "value"

            width: 72

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            padding: 2
            background: Rectangle {
                color: Material.color(Material.Grey, Material.Shade700)
                radius: 2
            }
            verticalAlignment: Text.AlignVCenter
            anchors.topMargin: 4
            font.pixelSize: 12
            horizontalAlignment: Text.AlignRight
        }

        StackLayout {
            id: controlStack

            anchors.top: valueLabel.bottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.topMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter

            currentIndex: root.isSwitch ? 1 : 0

            Slider {
                id: valueSlider

                orientation: Qt.Vertical

                onMoved: sliderMoved(valueSlider.value)
            }

            Switch {
                id: toggleSwitch

                onToggled: root.switchToggled(toggleSwitch.checked)
            }
        }
    }
}
