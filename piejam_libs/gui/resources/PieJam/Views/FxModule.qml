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

    ModelSubscription {
        target: root.model
        subscribed: root.visible
    }

    QtObject {
        id: private_

        readonly property bool bypassed: root.model && root.model.bypassed
        readonly property int contentType: root.model && root.model.content ? root.model.content.type : -1
    }

    RowLayout {
        anchors.fill: parent

        spacing: 0

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 8

            RowLayout {
                Layout.fillWidth: true

                Button {
                    Layout.preferredWidth: 24
                    Layout.preferredHeight: 36

                    icon.width: 24
                    icon.height: 24
                    icon.source: "qrc:///images/icons/power.svg"
                    display: AbstractButton.IconOnly
                    padding: 8

                    checkable: true
                    checked: !private_.bypassed

                    onClicked: if (root.model) root.model.toggleBypass()
                }

                HeaderLabel {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 24

                    text: root.model ? (root.model.chainName + " - " + root.model.name) : ""
                }
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: private_.contentType

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ParametersListView {
                        content: private_.contentType === FxModuleContent.Type.Generic ? root.model.content : null
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: FilterView {
                        content: private_.contentType === FxModuleContent.Type.Filter ? root.model.content : null
                        bypassed: private_.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ScopeView {
                        content: private_.contentType === FxModuleContent.Type.Scope ? root.model.content : null
                        bypassed: private_.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: SpectrumView {
                        content: private_.contentType === FxModuleContent.Type.Spectrum ? root.model.content : null
                        bypassed: private_.bypassed
                    }
                }
            }
        }

        ViewToolBar {
            Layout.fillHeight: true
        }
    }
}
