// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias perform: performStrip.model
    property alias edit: editStrip.model
    property alias fx: fxStrip.model
    property alias auxSend: auxSendStrip.model

    property bool deletable: true

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

            deletable: root.deletable
        }

        ChannelFxStrip {
            id: fxStrip
        }

        ChannelAuxSend {
            id: auxSendStrip
        }
    }
}
