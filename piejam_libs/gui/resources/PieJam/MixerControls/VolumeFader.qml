// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import PieJam.Items 1.0 as PJItems

import ".."
import "../Controls"
import "../Util/ColorExt.js" as ColorExt
import "../Util/DbConvert.js" as DbConvert

Item {
    id: root

    property var model: null
    property var scaleData

    implicitWidth: 50
    implicitHeight: 300

    Slider {
        id: slider

        anchors.fill: parent

        padding: 6

        value: root.model ? root.scaleData.dBToPosition(DbConvert.to_dB(root.model.value)) : 0

        orientation: Qt.Vertical

        background: Item {
            anchors.fill: parent

            RowLayout {
                anchors.fill: parent

                spacing: 2

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Repeater {
                        model: scaleData.ticks

                        delegate: Label {
                            anchors.left: parent ? parent.left : undefined
                            anchors.right: parent ? parent.right : undefined

                            y: (1 - modelData.position) * (parent.height - 12)

                            font.pixelSize: 10
                            font.bold: true

                            text: modelData.dB
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }

                PJItems.DbScale {
                    id: dbScale

                    Layout.preferredWidth: 8
                    Layout.fillHeight: true

                    scaleData: root.scaleData
                    color: Material.primaryTextColor
                    tickOffset: 6
                    edge: PJItems.DbScale.Edge.Right
                }
            }
        }

        handle: Rectangle {
            y: slider.visualPosition * slider.availableHeight + 6 - Screen.devicePixelRatio

            anchors.left: parent.left
            anchors.right: parent.right

            height: 2 + Screen.devicePixelRatio

            color: ColorExt.setAlpha(Material.accentColor, 0.6)
        }

        onMoved: {
            if (root.model) {
                var newVolume = root.scaleData.dBAt(slider.value)
                root.model.changeValue(DbConvert.from_dB(newVolume))
                Info.showParameterValue(root.model)
            }
        }
    }

    MidiAssignArea {
        id: midiAssign

        anchors.fill: parent

        model: root.model ? root.model.midi : null
    }
}


