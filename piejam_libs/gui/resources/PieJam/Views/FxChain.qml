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

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13

import "../FxChainControls"
import "../MixerControls"

TopPane {
    id: root

    property var model

    ListView {
        id: fxModules

        x: 5
        y: 7
        width: 627
        height: 418

        model: root.model.modules

        clip: true
        orientation: ListView.Horizontal
        spacing: 4

        delegate: FxChainModule {
            height: 412

            name: modelData.name
            parameters: modelData.parameters
        }

        footer: Item {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            implicitWidth: addButton.implicitWidth + 8

            Button {
                id: addButton

                implicitWidth: 32

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 4
                anchors.left: parent.left
                anchors.leftMargin: 4

                text: "+"
            }
        }
    }

    Frame {
        x: 638
        y: 7
        width: 153
        height: 418

        ComboBox {
            id: channelSelector

            x: 0
            y: 0
            width: 128
            height: 48

            model: root.model.channels
        }

        LevelMeterFader {
            id: levelMeterFader

            x: 0
            y: 54
            width: 128
            height: 340
        }
    }
}
