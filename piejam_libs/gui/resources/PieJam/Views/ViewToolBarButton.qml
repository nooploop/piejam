// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias iconSource: button.icon.source
    property alias text: button.text
    property bool checked: false

    signal clicked()

    Layout.preferredWidth: 32
    Layout.preferredHeight: 40
    Layout.alignment: Qt.AlignHCenter

    Button {
        id: button

        anchors.fill: parent

        icon.width: 24
        icon.height: 24

        font.pixelSize: 12

        checkable: true
        checked: root.checked

        onClicked: root.clicked()
    }
}
