// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import ".."

SubscribableItem {
    id: root

    property variant channels: [ "-" ]

    RowLayout {

        anchors.fill: parent
        anchors.margins: 4

        TextField {
            Layout.fillWidth: true

            text: root.model ? root.model.name : ""

            onEditingFinished: {
                root.model.changeName(text)
                focus = false
            }
        }

        ComboBox {
            Layout.preferredWidth: 64 + 6 + 64

            model: root.channels
            currentIndex: root.model ? root.model.monoChannel : -1
            visible: root.model && root.model.mono

            onActivated: if (root.model) root.model.changeMonoChannel(index)
        }

        ComboBox {
            Layout.preferredWidth: 64

            model: root.channels
            currentIndex: root.model ? root.model.stereoLeftChannel : -1
            visible: root.model && !root.model.mono

            onActivated: if (root.model) root.model.changeStereoLeftChannel(index)
        }

        ComboBox {
            Layout.preferredWidth: 64

            model: root.channels
            currentIndex: root.model ? root.model.stereoRightChannel : -1
            visible: root.model && !root.model.mono

            onActivated: if (root.model) root.model.changeStereoRightChannel(index)
        }

        Button {
            Layout.preferredWidth: 38

            text: "X"

            onClicked: if (root.model) root.model.remove()
        }
    }
}
