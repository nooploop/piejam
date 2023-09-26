// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property var perform: null
    property var edit: null
    property var fx: null

    property bool deletable: true

    implicitWidth: 132
    implicitHeight: 400

    StackLayout {
        anchors.fill: parent

        currentIndex: MixerViewSettings.mode

        ChannelPerformStrip {
            model: root.perform
        }

        ChannelEditStrip {
            model: root.edit

            deletable: root.deletable
        }

        ChannelFxStrip {
            model: root.fx
        }
    }
}
