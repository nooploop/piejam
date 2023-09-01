// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."
import "../Controls"

ViewPane {
    id: root

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
                  ? "<b>" +
                    fxList.currentEntry.name +
                    "</b><br><br>" +
                    (fxList.currentEntry.author !== ""
                        ? "<i>Author: " + fxList.currentEntry.author + "</i><br><br>"
                        : "") +
                    fxList.currentEntry.description
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
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            reuseItems: true

            model: root.model.entries


            delegate: Button {
                width: parent ? parent.width : implicitWidth
                height: 40

                text: model.item.name
                font.capitalization: Font.MixedCase
                highlighted: index == fxList.currentIndex
                flat: true

                onClicked: {
                    fxList.currentIndex = index
                    fxList.currentEntry = model.item
                }

                ModelSubscription {
                    target: model.item
                    subscribed: visible
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

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 8

        text: qsTr("Insert")

        enabled: fxList.currentIndex != -1

        onClicked: fxList.currentEntry.appendModule()
    }

    Button {
        id: cancelButton

        width: 96
        text: qsTr("Cancel")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 8

        onClicked: root.model.showMixer()
    }
}
