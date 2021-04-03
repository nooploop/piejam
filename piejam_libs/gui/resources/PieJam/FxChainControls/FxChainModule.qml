// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias name: nameLabel.text
    property alias type: moduleContent.currentIndex
    property alias parameters: parametersList.model
    property bool bypassed: false
    property alias moveLeftEnabled: moveLeftButton.enabled
    property alias moveRightEnabled: moveRightButton.enabled

    signal bypassButtonClicked()
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

        width: Math.max(bypassButton.width +
                        moveLeftButton.width +
                        moveRightButton.width +
                        nameLabel.implicitWidth +
                        swapButton.width +
                        deleteButton.width +
                        20, parametersList.width) + 2 * frame.padding
        height: bypassButton.height + 2 * frame.padding

        Button {
            id: bypassButton

            width: 24
            height: 35

            anchors.top: parent.top
            anchors.topMargin: -6

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/power.svg"
            display: AbstractButton.IconOnly
            padding: 8

            font.bold: true
            font.pixelSize: 12

            checkable: true
            checked: !root.bypassed

            onClicked: root.bypassButtonClicked()
        }

        Button {
            id: moveLeftButton
            width: 24
            height: 35

            anchors.top: parent.top
            anchors.topMargin: -6
            anchors.left: bypassButton.right
            anchors.leftMargin: 4

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

        Button {
            id: swapButton
            width: 24
            height: 35

            anchors.top: parent.top
            anchors.topMargin: -6
            anchors.left: moveRightButton.right
            anchors.leftMargin: 4

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/autorenew.svg"
            display: AbstractButton.IconOnly
            padding: 8

            font.bold: true
            font.pixelSize: 12

            onClicked: swapButtonClicked()
        }

        Label {
            id: nameLabel

            anchors.left: swapButton.right
            anchors.leftMargin: 4

            padding: 4
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            background: Rectangle {
                color: Material.primaryColor
                radius: 2
            }
            font.bold: true
            font.pixelSize: 12
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

        StackLayout {
            id: moduleContent

            anchors.top: nameLabel.bottom
            anchors.bottom: parent.bottom

            ParametersListView {
                id: parametersList
            }

            Label {
                text: "scope"
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
