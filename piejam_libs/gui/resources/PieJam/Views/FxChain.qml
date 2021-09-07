// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../Controls"
import "../FxChainControls"
import "../MixerControls"

ViewPane {
    id: root

    property var chainModel
    property alias browser: fxBrowser.model
    property alias chainIndex: fxChains.currentIndex

    StackView {
        id: stack

        anchors.fill: parent

        initialItem: fxChains

        onVisibleChanged: {
            if (!stack.visible)
                stack.pop(fxChains)
        }
    }

    StackList {
        id: fxChains

        visible: false
        model: chainModel

        delegate: Item {
            id: fxChain

            property var modelItem: model.item

            ListView {
                id: fxModules

                anchors.fill: parent
                anchors.margins: 8

                model: fxChain.modelItem ? fxChain.modelItem.modules : null

                clip: true
                orientation: ListView.Horizontal
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds
                spacing: 4

                delegate: FxChainModule {
                    anchors.top: if (parent) parent.top
                    anchors.bottom: if (parent) parent.bottom

                    name: model.item.name
                    bypassed: model.item.bypassed
                    content: model.item.content
                    moveLeftEnabled: model.item.canMoveLeft
                    moveRightEnabled: model.item.canMoveRight

                    onSwapButtonClicked: {
                        fxBrowser.addMode = FxBrowser.AddMode.Replace
                        fxBrowser.chainIndex = root.chainIndex
                        fxBrowser.insertPosition = index
                        stack.push(fxBrowser)
                    }
                    onDeleteButtonClicked: {
                        model.item.subscribed = false
                        model.item.deleteModule()
                    }
                    onAddButtonClicked: {
                        fxBrowser.addMode = FxBrowser.AddMode.Insert
                        fxBrowser.chainIndex = root.chainIndex
                        fxBrowser.insertPosition = index + 1
                        stack.push(fxBrowser)
                    }
                    onMoveLeftButtonClicked: model.item.moveLeft()
                    onMoveRightButtonClicked: model.item.moveRight()
                    onBypassButtonClicked: model.item.toggleBypass()

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
                            fxBrowser.chainIndex = root.chainIndex
                            fxBrowser.insertPosition = 0
                            stack.push(fxBrowser)
                        }
                    }
                }
            }

            Binding {
                when: root.chainModel
                target: model.item
                property: "subscribed"
                value: root.visible
            }
        }
    }

    FxBrowser {
        id: fxBrowser

        visible: false

        onCancelClicked: stack.pop()
        onAddClicked: stack.pop()
    }
}
