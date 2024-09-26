// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property real value: 0
    property bool bipolar: false
    property color color: Material.accentColor
    property int orientation: Qt.Horizontal
    property int direction: Qt.LeftToRight

    implicitWidth: 100
    implicitHeight: 100

    Rectangle {
        function validateRect(r) {
            return Qt.rect()
        }

        anchors.fill: parent

        color: Material.backgroundColor

        Rectangle {
            id: indicator

            color: root.color

            readonly property bool oh_: root.orientation === Qt.Horizontal
            readonly property bool dir_: root.direction === Qt.LeftToRight
            readonly property real pos_:
                root.bipolar * Math.min(0.5, dir_ * root.value + !dir_ * (1 - root.value)) + !root.bipolar * !dir_ * (1 - root.value)
            readonly property real len_: (1 - root.bipolar) * root.value + root.bipolar * Math.abs(root.value - 0.5)

            x: oh_ * parent.width * pos_
            y: !oh_ * parent.height * pos_
            width: parent.width * (oh_ * len_ + !oh_)
            height: parent.height * (!oh_ * len_ + oh_)
        }
    }
}
