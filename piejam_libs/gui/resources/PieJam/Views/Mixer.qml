// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

import ".."
import "../MixerControls"

ViewPane {
    id: root

    property var model

    signal fxButtonClicked(int index)

    Rectangle {
        id: leftBorderShadow

        width: 4
        anchors.left: parent.left

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        z: 100
        gradient: Gradient {

            orientation: Gradient.Horizontal

            GradientStop {
                position: 0
                color: "#ff000000"
            }

            GradientStop {
                position: 1
                color: "#00000000"
            }
        }
    }

    ChannelsListView {
        id: inputs

        anchors.left: parent.left
        anchors.right: mainChannelStrip.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 8
        anchors.leftMargin: 0

        model: root.model.inputChannels

        onFxButtonClicked: root.fxButtonClicked(index + 1)

        header: Item {
            width: 8

            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        footer: Item {
            width: 142

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            visible: MixerViewSettings.mode == MixerViewSettings.edit

            ChannelAddStrip {
                id: channelAddStrip

                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.rightMargin: 8

                name: privates.channelAddStripName

                onAddClicked: root.model.addChannel(channelAddStrip.name)
            }
        }

        QtObject {
            id: privates

            property string channelAddStripName: ""
        }

        onCountChanged: privates.channelAddStripName = "In " + (inputs.count + 1)
    }

    Rectangle {
        id: channelsSeparator

        width: 4

        anchors.right: mainChannelStrip.left
        anchors.rightMargin: 8
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        z: 100

        gradient: Gradient {

            orientation: Gradient.Horizontal

            GradientStop {
                position: 0
                color: "#00000000"
            }

            GradientStop {
                position: 1
                color: "#ff000000"
            }
        }
    }

    Loader {
        id: mainChannelStrip

        anchors.right: mixerToolbar.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 8

        active: root.model.mainChannel !== null

        sourceComponent: ChannelStrip {

            perform.model: root.model.mainChannel.perform
            perform.onFxButtonClicked: root.fxButtonClicked(0)

            edit.model: root.model.mainChannel.edit
            edit.deletable: false
        }

    }


    Rectangle {
        id: mixerToolbar

        width: 48

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        color: Material.color(Material.Grey, Material.Shade800)

        Button {
            id: editButton

            width: 32
            height: 44

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 4

            icon.width: 24
            icon.height: 24
            icon.source: "qrc:///images/icons/pencil.svg"
            display: AbstractButton.IconOnly

            checkable: true
            checked: MixerViewSettings.mode === MixerViewSettings.edit

            onClicked: {
                if (MixerViewSettings.mode === MixerViewSettings.edit)
                    MixerViewSettings.mode = MixerViewSettings.perform
                else
                    MixerViewSettings.mode = MixerViewSettings.edit
            }
        }
    }

    Timer {
        interval: 16
        running: root.visible && MixerViewSettings.mode === MixerViewSettings.perform
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
