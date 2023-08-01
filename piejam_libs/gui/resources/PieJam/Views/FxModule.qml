// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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

    property var model

    readonly property bool bypassed: root.model && root.model.bypassed

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }

    Frame {
        id: frame

        anchors.fill: parent
        anchors.margins: 8

        spacing: 0

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true

                Button {
                    id: bypassButton

                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 36

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/power.svg"
                    display: AbstractButton.IconOnly
                    padding: 8

                    checkable: true
                    checked: !root.bypassed

                    onClicked: if (root.model) root.model.toggleBypass()
                }

                HeaderLabel {
                    id: nameLabel

                    Layout.fillWidth: true
                    Layout.preferredHeight: 24

                    text: root.model ? root.model.name : ""
                }
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: {
                    if (root.model && root.model.content) {
                        switch (root.model.content.type) {
                        case FxModuleContent.Type.Generic:
                            return 0

                        case FxModuleContent.Type.Filter:
                            return 1

                        case FxModuleContent.Type.Scope:
                            return 2

                        case FxModuleContent.Type.Spectrum:
                            return 3

                        default:
                            console.log("unknown content type")
                            return -1
                        }
                    }
                    else
                    {
                        return -1
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ParametersListView {
                        content: root.model && root.model.content && root.model.content.type === FxModuleContent.Type.Generic
                                 ? root.model.content
                                 : null
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: FilterView {
                        content: root.model && root.model.content && root.model.content.type === FxModuleContent.Type.Filter
                                 ? root.model.content
                                 : null
                        bypassed: root.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ScopeView {
                        content: root.model && root.model.content && root.model.content.type === FxModuleContent.Type.Scope
                                 ? root.model.content
                                 : null
                        bypassed: root.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: SpectrumView {
                        content: root.model && root.model.content && root.model.content.type === FxModuleContent.Type.Spectrum
                                 ? root.model.content
                                 : null
                        bypassed: root.bypassed
                    }
                }
            }
        }
    }
}
