// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

Item {
    property alias perform: performStrip
    property alias edit: editStrip

    implicitWidth: 132
    implicitHeight: 400

    StackLayout {
        id: stripsStack

        anchors.fill: parent

        currentIndex: MixerViewSettings.mode == MixerViewSettings.perform ? 0 : 1

        ChannelPerformStrip {
            id: performStrip
        }

        ChannelEditStrip {
            id: editStrip
        }
    }

}
