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

    property var model

    signal addClicked()
    signal cancelClicked()

    Frame {
        id: infoFrame
        anchors.left: fxListFrame.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: addButton.top
        anchors.margins: 8

        Label {
            id: infoLabel
            anchors.fill: parent
            wrapMode: Text.WordWrap
            padding: 16
            textFormat: Text.RichText

            text: fxList.currentEntry
                  ? "<b>"+ fxList.currentEntry.name + "</b><br><br>" + fxList.currentEntry.description
                  : ""
        }
    }

    Frame {
        id: fxListFrame
        width: 300
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: addButton.top
        anchors.margins: 8

        ListView {
            id: fxList

            property var currentEntry

            anchors.fill: parent
            clip: true

            model: root.model.entries

            delegate: Button {
                width: parent.width
                height: 40

                text: model.item.name
                font.capitalization: Font.MixedCase
                highlighted: index == fxList.currentIndex

                onClicked: {
                    fxList.currentIndex = index
                    fxList.currentEntry = model.item
                }
            }

            section.property: "item.section"
            section.delegate: HeaderLabel {
                width: parent.width
                height: 32
                text: section
            }

            onModelChanged: fxList.currentIndex = -1
        }
    }

    Button {
        id: addButton
        width: 96
        text: qsTr("Add")
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 8

        enabled: fxList.currentEntry ? true : false

        onClicked: {
            fxList.currentEntry.addModule()
            root.addClicked()
        }
    }

    Button {
        id: cancelButton
        width: 96
        text: qsTr("Cancel")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 8

        onClicked: cancelClicked()
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
