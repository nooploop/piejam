// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import PieJam.Models 1.0 as PJModels

import ".."
import "../Controls"
import "../MixerControls"

ViewPane {
    id: root

    property var model

    Rectangle {
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

                name: "In " + (inputs.count + 1)

                onAddMonoClicked: root.model.addMonoChannel(channelAddStrip.name)
                onAddStereoClicked: root.model.addStereoChannel(channelAddStrip.name)
            }
        }
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

            edit.model: root.model.mainChannel.edit
            edit.deletable: false

            fx.model: root.model.mainChannel.fx
        }

    }

    ViewToolBar {
        id: mixerToolbar

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        ViewToolBarButton {
            iconSource: "qrc:///images/icons/pencil.svg"
            checked: MixerViewSettings.mode === MixerViewSettings.edit

            onClicked: MixerViewSettings.switchMode(MixerViewSettings.edit)
        }

        ViewToolBarButton {
            text: "FX"
            checked: MixerViewSettings.mode === MixerViewSettings.fx

            onClicked: MixerViewSettings.switchMode(MixerViewSettings.fx)
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Timer {
        interval: 16
        running: root.visible && MixerViewSettings.mode === MixerViewSettings.perform
        repeat: true
        onTriggered: root.model.requestLevelsUpdate()
    }

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }
}
