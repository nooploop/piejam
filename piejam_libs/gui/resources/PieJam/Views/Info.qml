// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

import QtQuick 2.12
import QtQuick.Controls 2.12

TopPane {
    id: root

    // model:
    //   .audioLoad : real
    //   .xruns : int
    //   .logData : list<string>
    property var model

    Label {
        id: lblAudioLoad
        x: 8
        y: 8
        width: 100
        height: 18
        text: qsTr("Audio Load:")
    }

    Label {
        id: lblXRuns
        x: 8
        y: 32
        width: 100
        height: 18
        text: qsTr("Dropouts:")
    }

    Label {
        id: lblAudioLoadValue
        x: 108
        y: 8
        width: 100
        height: 18
        text: (root.model.audioLoad * 100).toFixed(1) + " %"
    }

    Label {
        id: lblXRunsValue
        x: 108
        y: 32
        width: 100
        height: 18
        color: root.model.xruns === 0 ? Qt.rgba(0, 1, 0, 1) : Qt.rgba(1, 0, 0, 1)
        text: root.model.xruns
    }

    Frame {
        x: 8
        y: 56
        width: 784
        height: 366

        ListView {
            id: lstLog
            x: 0
            y: 24
            width: 760
            height: 318

            model: root.model.logData

            delegate: Label {
                width: 688
                height: 20

                text: modelData
            }
        }

        Label {
            id: lblLog
            x: 0
            y: 0
            width: 100
            height: 18
            text: qsTr("Log")
            font.bold: true
        }
    }

    Timer {
        interval: 80
        running: root.visible
        repeat: true
        onTriggered: root.model.requestUpdate()
    }

    onVisibleChanged: root.model.requestUpdate()
}
