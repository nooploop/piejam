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
    property alias browser: fxBrowser.model

    StackView {
        id: stack

        anchors.fill: parent

        initialItem: fxChain
    }

    Item {
        id: fxChain

        visible: false

        ListView {
            id: fxModules

            anchors.left: parent.left
            anchors.right: levelMeterFrame.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 8

            model: root.model.modules

            clip: true
            orientation: ListView.Horizontal
            spacing: 4

            delegate: FxChainModule {
                anchors.top: if (parent) parent.top
                anchors.bottom: if (parent) parent.bottom

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
                    anchors.bottomMargin: -1
                    anchors.left: parent.left
                    anchors.leftMargin: 4

                    visible: root.model.buses.focused !== -1

                    text: "+"

                    onClicked: stack.push(fxBrowser)
                }
            }
        }

        Frame {
            id: levelMeterFrame

            width: levelMeterFader.width + levelMeterFrame.leftPadding + levelMeterFrame.rightPadding

            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 8

            ComboBox {
                id: channelSelector

                height: 48

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                model: root.model.buses.elements
                currentIndex: root.model.buses.focused

                displayText: root.model.buses.focused === -1 ? "[Select]" : currentText

                onActivated: root.model.selectBus(index)
                onModelChanged: currentIndex = root.model.buses.focused
            }

            CompactLevelMeterFader {
                id: levelMeterFader

                anchors.top: channelSelector.bottom
                anchors.bottom: parent.bottom
                anchors.topMargin: 4

                levelLeft: root.model.levelLeft
                levelRight: root.model.levelRight
                volume: root.model.volume

                enabled: root.model.buses.focused !== -1

                onFaderMoved: root.model.changeVolume(newVolume)
            }
        }

    }

    FxBrowser {
        id: fxBrowser

        visible: false

        onCancelClicked: stack.pop()
        onAddClicked: {
            stack.pop()
            fxModules.positionViewAtEnd()
        }
    }

    Timer {
        interval: 16
        running: levelMeterFader.visible
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
