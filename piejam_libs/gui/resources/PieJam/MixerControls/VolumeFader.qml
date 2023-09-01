// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import ".."
import "../Util/ColorExt.js" as ColorExt
import "../Util/DbConvert.js" as DbConvert

Item {
    id: root

    property real volume
    property var scaleData

    signal moved(real newVolume)

    implicitWidth: 50
    implicitHeight: 300

    Slider {
        id: slider

        anchors.fill: parent

        padding: 6

        value: root.volume

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
            var newVolume = root.scaleData.dBAt(slider.value)
            root.moved(slider.value)
            Info.show("<b>Volume:</b> " + newVolume.toFixed(1) + " dB")
        }
    }
}


