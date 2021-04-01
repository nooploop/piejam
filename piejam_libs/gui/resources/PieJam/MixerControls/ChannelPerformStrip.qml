// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import ".."
import "../Controls"

Item {
    id: root

    property var model

    signal fxButtonClicked()

    implicitWidth: 132
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        HeaderLabel {
            id: title

            height: 24

            anchors.top: parent.top
            anchors.right: fxButton.left
            anchors.rightMargin: 4
            anchors.left: parent.left

            text: root.model ? root.model.name : ""
        }

        BipolarSlider {
            id: panControls

            height: 40

            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: title.bottom

            value: root.model ? root.model.panBalance : 0

            onMoved: {
                root.model.changePanBalance(panControls.value)
                Info.quickTip = (root.model.mono ? "<b>Pan:</b> " : "<b>Balance:</b> ")
                        + panControls.value.toFixed(2)
            }

            MidiAssignArea {
                id: midiAssignPan

                model: root.model ? root.model.panMidi : null

                anchors.fill: parent
                anchors.topMargin: 1
                anchors.bottomMargin: 1
            }
        }

        LevelMeterFader {
            id: levelMeterFader

            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: channelControls.top
            anchors.top: panControls.bottom

            volume: root.model ?  root.model.volume : 0
            levelLeft: root.model ? root.model.levelLeft : 0
            levelRight: root.model ? root.model.levelRight : 0

            muted: root.model ? (root.model.mute || root.model.mutedBySolo) : false

            volumeMidi.model: root.model ? root.model.volumeMidi : null

            onFaderMoved: root.model.changeVolume(newVolume)
        }

        ChannelControls {
            id: channelControls

            height: 32

            anchors.right: parent.right
            anchors.bottom: parent.bottom

            mute: root.model ? root.model.mute : false
            solo: root.model ? root.model.solo : false

            muteMidi.model: root.model ? root.model.muteMidi : null
            soloMidi.model: root.model ? root.model.soloMidi : null

            onMuteToggled: root.model.changeMute(!root.model.mute)
            onSoloToggled: root.model.changeSolo(!root.model.solo)
        }

        Button {
            id: fxButton

            width: 24
            height: 36

            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: parent.top
            anchors.topMargin: -6

            text: qsTr("fx")

            padding: 6

            onClicked: {
                root.model.focusFxChain()
                root.fxButtonClicked()
            }
        }
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
        restoreMode: Binding.RestoreBinding
    }
}
