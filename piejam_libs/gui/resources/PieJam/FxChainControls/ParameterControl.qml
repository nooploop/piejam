// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0

import ".."
import "../Controls"

Item {
    id: root

    property var paramModel: null

    implicitWidth: 82

    Rectangle {
        id: headerRect

        anchors.top: parent.top
        anchors.right: parent.right

        width: 82
        height: 70

        color: Material.color(Material.Grey, Material.Shade800)
        radius: 2

        HeaderLabel {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            height: 40

            text: root.paramModel ? root.paramModel.name : "name"

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            wrapMode: Text.WordWrap

            backgroundColor: Material.color(Material.Grey, Material.Shade800)
        }

        Label {
            text: root.paramModel ? root.paramModel.valueString : "value"

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 4

            padding: 2
            background: Rectangle {
                color: Material.color(Material.Grey, Material.Shade700)
                radius: 2
            }

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            textFormat: Text.PlainText
            font.pixelSize: 12
        }
    }

    Rectangle {
        id: sliderRect

        anchors.top: headerRect.bottom
        anchors.topMargin: 4
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        color: Material.color(Material.Grey, Material.Shade800)
        radius: 2

        StackLayout {
            id: controlStack

            anchors.fill: parent
            anchors.margins: 8

            currentIndex: root.paramModel ? root.paramModel.type : -1

            ParameterTouchstrip {
                paramModel: root.paramModel
            }

            IntSlider {
                paramModel: root.paramModel
            }

            ParameterSwitch {
                paramModel: root.paramModel
            }
        }
    }
}
