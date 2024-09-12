// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {

    id: root

    property alias nameLabelWidth: nameLabel.width
    property alias nameLabelText: nameLabel.text
    property alias model: comboBox.model
    property alias currentIndex: comboBox.currentIndex
    property string emptyText: qsTr("Not Available")
    property string unselectedText: qsTr("Select...")

    signal optionSelected(int index)

    implicitWidth: 300
    implicitHeight: 56

    Frame {
        id: frame

        anchors.fill: parent

        spacing: 0

        topPadding: 2
        bottomPadding: 2

        RowLayout {
            anchors.fill: parent

            Label {
                id: nameLabel

                Layout.preferredWidth: 128
                Layout.fillHeight: true

                verticalAlignment: Text.AlignVCenter
                textFormat: Text.PlainText
                font.pixelSize: 18
            }

            ComboBox {
                id: comboBox

                Layout.fillWidth: true
                Layout.fillHeight: true

                displayText: comboBox.count == 0
                             ? emptyText
                             : (comboBox.currentIndex == -1 ? unselectedText : comboBox.currentText)
                enabled: comboBox.count != 0

                onActivated: root.optionSelected(index)
            }
        }
    }

}
