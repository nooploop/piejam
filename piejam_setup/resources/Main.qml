// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import PieJamSetup 1.0

Window {
    id: root

    width: 800
    height: 480

    visible: true

    Image {
        anchors.fill: parent

        source: "boot_logo.png"

        ListView {
            width: 256

            anchors.top: parent.top
            anchors.topMargin: 280
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            interactive: false

            model: mainModel.tasks

            delegate: Item {
                width: parent ? parent.width : undefined
                height: 32

                RowLayout {
                    anchors.fill: parent

                    Label {
                        Layout.fillWidth: true

                        text: model.item.description
                    }

                    Label {
                        Layout.preferredWidth: 64

                        text: statusToString(model.item.status)
                        color: statusToColor(model.item.status)

                        function statusToColor(status) {
                            switch (status)
                            {
                            case Task.Status.Initial:
                                return "yellow"

                            case Task.Status.Running:
                                return "yellow"

                            case Task.Status.Done:
                                return "green"

                            default:
                                return "red"
                            }
                        }

                        function statusToString(status) {
                            switch (status)
                            {
                            case Task.Status.Initial:
                                return "WAIT"

                            case Task.Status.Running:
                                return "EXECUTE"

                            case Task.Status.Done:
                                return "DONE"

                            case Task.Status.Failed:
                                return "FAILED"

                            case Task.Status.Skipped:
                                return "SKIP"

                            default:
                                return ""
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            mainModel.tasks.run()
        }
    }
}
