// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    property alias perform: performStrip
    property alias edit: editStrip
    property alias fx: fxStrip

    implicitWidth: 132
    implicitHeight: 400

    StackLayout {
        anchors.fill: parent

        currentIndex: MixerViewSettings.mode

        ChannelPerformStrip {
            id: performStrip
        }

        ChannelEditStrip {
            id: editStrip
        }

        ChannelFxStrip {
            id: fxStrip
        }
    }
}
