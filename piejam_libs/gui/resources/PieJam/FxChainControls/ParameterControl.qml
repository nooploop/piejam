// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70

            color: Material.color(Material.Grey, Material.Shade800)
            radius: 2

            ColumnLayout {
                anchors.fill: parent

                HeaderLabel {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    text: root.paramModel ? root.paramModel.name : "name"

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    textFormat: Text.PlainText
                    wrapMode: Text.WordWrap

                    backgroundColor: Material.color(Material.Grey, Material.Shade800)
                }

                Label {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    text: root.paramModel ? root.paramModel.valueString : "value"

                    padding: 2
                    background: Rectangle {
                        color: Material.color(Material.Grey, Material.Shade700)
                        radius: 2
                    }

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    textFormat: Text.PlainText
                    font.pixelSize: 12

                    MouseArea {
                        anchors.fill: parent

                        onDoubleClicked: {
                            if (root.paramModel) {
                                root.paramModel.resetToDefault()
                                Info.showParameterValue(root.paramModel)
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: Material.color(Material.Grey, Material.Shade800)
            radius: 2

            StackLayout {
                anchors.fill: parent
                anchors.margins: 8

                currentIndex: root.paramModel ? root.paramModel.type : -1

                ParameterTouchstrip {
                    model: root.paramModel
                }

                IntSlider {
                    model: root.paramModel
                }

                ParameterSwitch {
                    model: root.paramModel
                }
            }
        }
    }
}
