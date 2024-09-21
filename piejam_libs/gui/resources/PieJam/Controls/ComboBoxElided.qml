// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ComboBox {
    id: root

    property string displayTextToElide: ""
    property int elideMode: Qt.ElideNone

    FontMetrics {
        id: fontMetrics

        font: root.font
    }

    // TODO: how to derive proper contentItem.width
    displayText: fontMetrics.elidedText(root.displayTextToElide, root.elideMode, contentItem.width - 4)
}
