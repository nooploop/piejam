// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2
import QtQuick.Controls 2
import QtQuick.Controls.Material 2

import ".."

Item {
    id: root

    property var model

    implicitWidth: 100
    implicitHeight: 100

    Label {
        id: assignmentLabel

        readonly property color unassignedColor: "#80eeb72b"
        readonly property color unassignedLearnColor: "#30eeb72b"
        readonly property color unassignedBorderColor: "#eeb72b"
        readonly property color assignedColor: "#806bea04"
        readonly property color assignedLearnColor: "#306bea04"
        readonly property color assignedBorderColor: "#6bea04"

        readonly property bool assigned: root.model ? root.model.assignment !== "" : false
        readonly property color backgroundColor: assigned ? assignedColor : unassignedColor
        readonly property color backgroundLearnColor: assigned ? assignedLearnColor : unassignedLearnColor
        readonly property color borderColor: assigned ? assignedBorderColor : unassignedBorderColor

        anchors.fill: parent

        font.pixelSize: 11
        font.bold: true
        padding: 2
        color: assigned ? assignedBorderColor : unassignedBorderColor
        text: root.model ? root.model.assignment : ""

        background: Rectangle {
            id: background

            color: assignmentLabel.backgroundColor
            border.color: assignmentLabel.borderColor
            border.width: 2

            SequentialAnimation on color {
                running: midiAssign.learning
                loops: Animation.Infinite

                ColorAnimation {
                    target: background
                    property: "color"
                    from: assignmentLabel.backgroundColor
                    to: assignmentLabel.backgroundLearnColor
                    duration: 500
                }

                ColorAnimation {
                    target: background
                    property: "color"
                    from: assignmentLabel.backgroundLearnColor
                    to: assignmentLabel.backgroundColor
                    duration: 500
                }

                onStopped: background.color = Qt.binding(function() { return assignmentLabel.backgroundColor })
            }
        }
    }

    MouseArea {
        id: clickArea

        anchors.fill: parent

        onClicked: {
            midiAssign.learning
                   ? MidiLearn.stop()
                   : MidiLearn.start(midiAssign)
        }
    }

    MidiAssignable {
        id: midiAssign

        property bool learning: false

        onLearningStarted: {
            midiAssign.learning = true;
            if (root.model)
                root.model.startLearn()
        }

        onLearningStopped: {
            midiAssign.learning = false
            if (root.model)
                root.model.stopLearn()
        }
    }

    enabled: MidiLearn.active
    visible: MidiLearn.active

    onModelChanged: {
        if (root.model)
            root.model.subscribed = Qt.binding(function() { return root.visible && MidiLearn.active })
    }
}
