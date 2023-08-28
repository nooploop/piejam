// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQml.Models 2.15

import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"

Item {
    id: root

    property var model

    implicitWidth: 132

    Frame {
        id: frame

        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            HeaderLabel {
                id: title

                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? root.model.name : ""
            }

            ListView {
                id: fxModulesList

                Layout.fillWidth: true
                Layout.fillHeight: true

                clip: true
                boundsBehavior: Flickable.StopAtBounds
                boundsMovement: Flickable.StopAtBounds

                model: root.model ? root.model.fxChain : null

                delegate: Item {
                    id: delegateRoot

                    height: 36

                    anchors.left: parent ? parent.left : undefined
                    anchors.right: parent ? parent.right : undefined

                    Rectangle {
                        id: content

                        width: delegateRoot.width
                        height: 32
                        anchors.verticalCenter: delegateRoot.verticalCenter

                        border.color: model.item.focused ? Material.primaryColor : Material.frameColor
                        border.width: 2
                        color: Material.backgroundColor
                        radius: 4

                        RowLayout {
                            anchors.fill: parent

                            Label {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                padding: 4
                                text: model.item.name
                                verticalAlignment: Label.AlignVCenter

                                elide: Text.ElideRight

                                MouseArea {
                                    id: nameMouseArea

                                    anchors.fill: parent

                                    onClicked: model.item.focused ? model.item.showFxModule() : model.item.focus()
                                }
                            }

                            Rectangle {
                                Layout.preferredWidth: 24
                                Layout.fillHeight: true

                                visible: model.item.focused

                                color: Material.primaryColor
                                radius: 4

                                Label {
                                    anchors.fill: parent

                                    text: "X"
                                    font.bold: true

                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                MouseArea {
                                    id: deleteMouseArea

                                    anchors.fill: parent

                                    onClicked: model.item.remove()
                                }
                            }
                        }
                    }


                    ModelSubscription {
                        target: model.item
                        subscribed: visible
                    }
                }

            }

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    display: AbstractButton.IconOnly
                    icon.source: "qrc:///images/icons/chevron-up.svg"

                    enabled: root.model && root.model.canMoveUpFxModule

                    onClicked: root.model.moveUpFxModule()
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    text: "+"

                    onClicked: root.model.appendFxModule()
                }

                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    display: AbstractButton.IconOnly
                    icon.source: "qrc:///images/icons/chevron-down.svg"

                    enabled: root.model && root.model.canMoveDownFxModule

                    onClicked: root.model.moveDownFxModule()
                }
            }
       }
    }

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }
}
