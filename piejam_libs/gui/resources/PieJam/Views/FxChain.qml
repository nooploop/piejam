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

            name: model.item.name
            parameters: model.item.parameters

            onDeleteButtonClicked: root.model.deleteModule(index)

            Binding {
                target: model.item
                property: "subscribed"
                value: visible
            }
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

                visible: root.model.selectedBus !== -1

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

            currentIndex: root.model.selectedBus
            model: root.model.buses

            textRole: "display"

            onActivated: root.model.selectBus(index)
        }

        LevelMeterFader {
            id: levelMeterFader

            x: 0
            y: 54
            width: 128
            height: 340

            levelLeft: root.model.levelLeft
            levelRight: root.model.levelRight
            volume: root.model.volume

            enabled: channelSelector.currentIndex != -1

            onFaderMoved: root.model.changeVolume(newVolume)
        }
    }

    Timer {
        interval: 16
        running: root.visible
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
    }
}
