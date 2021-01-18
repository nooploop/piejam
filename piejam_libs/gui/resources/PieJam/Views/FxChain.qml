// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

            anchors.fill: parent
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
                moveLeftEnabled: model.item.canMoveLeft
                moveRightEnabled: model.item.canMoveRight

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
                onMoveLeftButtonClicked: model.item.moveLeft()
                onMoveRightButtonClicked: model.item.moveRight()

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
