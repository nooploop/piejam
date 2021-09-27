// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias value: slider.value
    property alias from: slider.from
    property alias to: slider.to

    signal changeValue(real newValue)

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        Button {
            id: upButton

            display: AbstractButton.IconOnly
            icon.source: "qrc:///images/icons/chevron-up.svg"

            Layout.maximumHeight: 40
            Layout.fillWidth: true

            onClicked: root.changeValue(Math.min(root.value + 1, root.to))
        }

        Slider {
            id: slider

            stepSize: 1
            orientation: Qt.Vertical

            Layout.fillWidth: true
            Layout.fillHeight: true

            onMoved: {
                root.changeValue(slider.value)
            }
        }

        Button {
            id: downButton

            display: AbstractButton.IconOnly
            icon.source: "qrc:///images/icons/chevron-down.svg"

            Layout.maximumHeight: 40
            Layout.fillWidth: true

            onClicked: root.changeValue(Math.max(root.value - 1, root.from))
        }
    }
}
