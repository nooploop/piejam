// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQml 2.15

import PieJam.Models 1.0

import ".."
import "../Controls"
import "../FxChainControls"

ViewPane {
    id: root

    QtObject {
        id: private_

        readonly property bool bypassed: root.model && root.model.bypassed
        readonly property var content: root.model ? root.model.content : null
        readonly property var contentType: private_.content ? private_.content.type : null
    }

    Material.primary: root.model ? root.model.color : Material.Pink
    Material.accent: root.model ? root.model.color : Material.Pink

    RowLayout {
        anchors.fill: parent

        spacing: 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8

            HeaderLabel {
                Layout.fillWidth: true
                Layout.preferredHeight: 24

                text: root.model ? (root.model.chainName + " - " + root.model.name) : ""
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: private_.contentType ? FxModuleRegistry.indexOf(private_.contentType) : -1

                Repeater {
                    model: FxModuleRegistry.items

                    delegate: BusyLoader {
                        id: fxModuleViewLoader

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        source: modelData.viewSource

                        Binding {
                            target: fxModuleViewLoader.item
                            property: "model"
                            value: private_.contentType === modelData.fxType ? private_.content : null
                        }

                        Binding {
                            target: fxModuleViewLoader.item
                            property: "bypassed"
                            value: private_.bypassed
                        }
                    }
                }
            }
        }

        ViewToolBar {
            Layout.fillHeight: true

            ViewToolBarButton {
                iconSource: "qrc:///images/icons/power.svg"
                checked: !private_.bypassed

                onClicked: if (root.model) root.model.toggleBypass()
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
