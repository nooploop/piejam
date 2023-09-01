// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import ".."
import "../Controls"
import "../Util/ColorExt.js" as ColorExt
import "../Util/DbConvert.js" as DbConvert

SubscribableItem {
    id: root

    property var scaleData

    implicitWidth: 50
    implicitHeight: 300

    Slider {
        id: slider

        anchors.fill: parent

        padding: 6

        value: root.model ? root.scaleData.dBToPosition(DbConvert.linToDb(root.model.value)) : 0

        orientation: Qt.Vertical

        background: DbScale {
            id: dbScale

            anchors.fill: parent

            padding: 6

            orientation: DbScale.Orientation.Right
            backgroundColor: Material.backgroundColor

            scaleData: root.scaleData
        }

        handle: Rectangle {
            x: slider.leftPadding + slider.availableWidth / 2 - width / 2
            y: slider.visualPosition * slider.availableHeight + height

            width: root.width
            height: 4

            color: ColorExt.setAlpha(Material.accentColor, 0.6)
        }

        onMoved: {
            if (root.model) {
                var newVolume = root.scaleData.dBAt(slider.value)
                root.model.changeValue(DbConvert.dbToLin(newVolume))
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


