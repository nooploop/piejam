// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam 1.0
import PieJam.Controls 1.0
import PieJam.FxChainControls 1.0
import PieJam.Models 1.0 as PJModels
import PieJam.Util 1.0

SubscribableItem {
    id: root

    property bool bypassed: false

    implicitWidth: 636

    QtObject {
        id: private_

        readonly property color fromColor: Material.color(Material.Green)
        readonly property color toColor: Material.color(Material.Red)
        readonly property bool validPitch: root.model && root.model.detectedFrequency !== 0
        readonly property color primaryColor: validPitch
                                    ? ColorExt.mapNormalized(Math.abs(root.model.detectedCents) / 50, fromColor, toColor)
                                    : Material.secondaryTextColor
        readonly property color secondaryColor: validPitch
                                    ? Material.primaryTextColor
                                    : Material.secondaryTextColor
    }

    ColumnLayout {
        anchors.fill: parent

        Label {
            Layout.fillWidth: true

            text: private_.validPitch ? root.model.detectedPitch : "--"

            color: private_.primaryColor

            horizontalAlignment: Qt.AlignHCenter
            font.pixelSize: 96
            font.bold: true
        }

        ValueIndicatorBar {
            Layout.fillWidth: true
            Layout.leftMargin: 32
            Layout.rightMargin: 32
            Layout.preferredHeight: 16

            color: private_.primaryColor

            bipolar: true
            value: private_.validPitch ? (root.model.detectedCents + 50) / 100 : 0.5
        }

        Label {
            Layout.fillWidth: true

            text: (private_.validPitch ? root.model.detectedCents : "-") + " ct"

            color: private_.secondaryColor

            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Qt.AlignHCenter
        }

        Label {
            Layout.fillWidth: true

            text: "(" + (private_.validPitch ? root.model.detectedFrequency : 0).toFixed(2) + " Hz)"

            color: private_.secondaryColor

            font.pixelSize: 24
            horizontalAlignment: Qt.AlignHCenter
        }
    }
}
