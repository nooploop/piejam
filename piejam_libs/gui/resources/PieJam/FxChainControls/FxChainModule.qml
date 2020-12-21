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

Item {
    id: root

    property alias name: nameLabel.text
    property alias parameters: parametersList.model

    signal swapButtonClicked()
    signal deleteButtonClicked()
    signal addButtonClicked()

    implicitWidth: frame.width + addButton.width + 4
    implicitHeight: frame.height

    Frame {
        id: frame

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: Math.max(nameLabel.implicitWidth + swapButton.width + deleteButton.width + 12, parametersList.implicitWidth) + 2 * frame.padding
        implicitHeight: nameLabel.implicitHeight + parametersList.implicitHeight + 2 * frame.padding

        Label {
            id: nameLabel

            padding: 4
            verticalAlignment: Text.AlignVCenter
            background: Rectangle {
                color: Material.primaryColor
                radius: 2
            }
            font.bold: true
            font.pixelSize: 12
        }

        Button {
            id: swapButton
            width: 24
            height: 35

            text: "s"
            anchors.right: deleteButton.left
            anchors.top: parent.top

            font.bold: true
            font.pixelSize: 12
            anchors.rightMargin: 4
            anchors.topMargin: -6

            onClicked: swapButtonClicked()
        }

        Button {
            id: deleteButton
            width: 24
            height: 35

            text: "x"
            anchors.right: parent.right
            anchors.top: nameLabel.top
            font.bold: true
            font.pixelSize: 12
            anchors.topMargin: -6

            onClicked: deleteButtonClicked()
        }

        ListView {
            id: parametersList
            anchors.top: nameLabel.bottom
            anchors.bottom: parent.bottom

            orientation: Qt.Horizontal

            spacing: 4

            implicitWidth: contentWidth

            delegate: ParameterControl {
                name: model.item.name
                value: model.item.value
                valueText: model.item.valueString
                switchValue: model.item.switchValue

                sliderMin: model.item.stepped ? model.item.minValue : 0.0
                sliderMax: model.item.stepped ? model.item.maxValue : 1.0
                sliderStep: model.item.stepped ? 1.0 : 0.0

                isSwitch: model.item.isSwitch

                height: parametersList.height

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

    Button {
        id: addButton

        width: 32

        text: "+"
        anchors.left: frame.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 4

        onClicked: root.addButtonClicked()
    }
}

/*##^##
Designer {
    D{i:0;formeditorColor:"#000000";height:300;width:400}
}
##^##*/
