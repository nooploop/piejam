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

import "../FxChainControls"
import "../MixerControls"

TopPane {
    id: root

    property var model
    property alias browser: fxBrowser.model

    StackView {
        id: stack

        anchors.fill: parent

        initialItem: fxChain

        onVisibleChanged: {
            if (!stack.visible)
                stack.pop(fxChain)
        }
    }

    Item {
        id: fxChain

        visible: false

        ListView {
            id: fxModules
            x: 8
            y: 8
            width: 784
            height: 416

            anchors.margins: 8

            model: root.model.modules

            clip: true
            orientation: ListView.Horizontal
            boundsBehavior: Flickable.StopAtBounds
            boundsMovement: Flickable.StopAtBounds
            spacing: 4

            delegate: FxChainModule {
                anchors.top: if (parent) parent.top
                anchors.bottom: if (parent) parent.bottom

                name: model.item.name
                parameters: model.item.parameters

                onSwapButtonClicked: {
                    fxBrowser.addMode = FxBrowser.AddMode.Replace
                    fxBrowser.insertPosition = index
                    stack.push(fxBrowser)
                }
                onDeleteButtonClicked: model.item.deleteModule()
                onAddButtonClicked: {
                    fxBrowser.addMode = FxBrowser.AddMode.Insert
                    fxBrowser.insertPosition = index + 1
                    stack.push(fxBrowser)
                }

                Binding {
                    target: model.item
                    property: "subscribed"
                    value: visible
                }
            }

            header: Item {
                anchors.top: parent.top
                anchors.bottom: parent.bottom

                width: addButton.width + 4

                Button {
                    id: addButton

                    width: 32

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    text: "+"

                    onClicked:{
                        fxBrowser.addMode = FxBrowser.AddMode.Insert
                        fxBrowser.insertPosition = 0
                        stack.push(fxBrowser)
                    }
                }
            }
        }
    }

    FxBrowser {
        id: fxBrowser

        visible: false

        onCancelClicked: stack.pop()
        onAddClicked: stack.pop()
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
