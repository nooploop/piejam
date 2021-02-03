// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

Item {
    id: root

    property alias name: nameLabel.text
    property alias parameters: parametersList.model
    property alias moveLeftEnabled: moveLeftButton.enabled
    property alias moveRightEnabled: moveRightButton.enabled

    signal swapButtonClicked()
    signal deleteButtonClicked()
    signal addButtonClicked()
    signal moveLeftButtonClicked()
    signal moveRightButtonClicked()

    implicitWidth: frame.width + addButton.width + 4
    implicitHeight: frame.height

    Frame {
        id: frame

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        width: Math.max(moveLeftButton.width +
                        moveRightButton.width +
                        nameLabel.implicitWidth +
                        swapButton.width +
                        deleteButton.width +
                        16, parametersList.implicitWidth) + 2 * frame.padding
        height: nameLabel.implicitHeight + parametersList.implicitHeight + 2 * frame.padding

        Button {
            id: moveLeftButton
            width: 24
            height: 35

            anchors.top: parent.top
            anchors.topMargin: -6

            text: "<"

            font.bold: true
            font.pixelSize: 12

            onClicked: root.moveLeftButtonClicked()
        }

        Button {
            id: moveRightButton
            width: 24
            height: 35

            anchors.top: parent.top
            anchors.topMargin: -6
            anchors.left: moveLeftButton.right
            anchors.leftMargin: 4

            text: ">"

            font.bold: true
            font.pixelSize: 12

            onClicked: root.moveRightButtonClicked()
        }

        Label {
            id: nameLabel

            anchors.left: moveRightButton.right
            anchors.leftMargin: 4

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

            anchors.right: deleteButton.left
            anchors.rightMargin: 4
            anchors.top: parent.top
            anchors.topMargin: -6

            text: "s"

            font.bold: true
            font.pixelSize: 12

            onClicked: swapButtonClicked()
        }

        Button {
            id: deleteButton
            width: 24
            height: 35

            anchors.right: parent.right
            anchors.top: nameLabel.top
            anchors.topMargin: -6

            text: "x"

            font.bold: true
            font.pixelSize: 12

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

                midi.model: model.item.midi

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
