// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."
import "../Controls"

Item {
    id: root

    property var model: privates.nullModel

    property alias meterMuted: levelMeterFader.muted

    signal soloToggled()
    signal fxButtonClicked()

    implicitWidth: 150
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        HeaderLabel {
            id: title

            width: 96
            height: 24

            anchors.top: parent.top
            anchors.left: parent.left

            text: root.model.name
        }

        BipolarSlider {
            id: panControls

            height: 40

            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: title.bottom

            value: root.model.panBalance

            onMoved: {
                root.model.changePanBalance(panControls.value)
                Info.quickTip = (root.model.mono ? "<b>Pan:</b> " : "<b>Balance:</b> ")
                        + panControls.value.toFixed(2)
            }

            MidiAssignArea {
                id: midiAssignPan

                model: root.model.panMidi

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

            volume: root.model.volume
            levelLeft: root.model.levelLeft
            levelRight: root.model.levelRight

            volumeMidi.model: root.model.volumeMidi

            onFaderMoved: root.model.changeVolume(newVolume)
        }

        ChannelControls {
            id: channelControls

            height: 32

            anchors.right: parent.right
            anchors.bottom: parent.bottom

            mute: root.model.mute
            solo: root.model.solo

            muteMidi.model: root.model.muteMidi

            onMuteToggled: root.model.changeMute(!root.model.mute)
            onSoloToggled: root.soloToggled()
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

    onModelChanged: {
        if (!root.model) {
            root.model = privates.nullModel
        } else {
            root.model.subscribed = Qt.binding(function () { return root.visible })
        }
    }

    QtObject {
        id: privates

        readonly property var nullModel: ({
            name: "",
            mono: false,
            volume: 0,
            levelLeft: 0,
            levelRight: 0,
            panBalance: 0,
            mute: false,
            solo: false,
            volumeMidi: null,
            panMidi: null,
            muteMidi: null
        })
    }
}
