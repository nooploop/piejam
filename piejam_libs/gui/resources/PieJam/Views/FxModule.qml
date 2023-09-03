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

    QtObject {
        id: private_

        readonly property bool bypassed: root.model && root.model.bypassed
        readonly property var content: root.model ? root.model.content : null
        readonly property int contentType: private_.content ? private_.content.type : -1
    }

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

                currentIndex: private_.contentType

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ParametersListView {
                        model: private_.contentType === FxModuleContent.Type.Generic ? private_.content : null
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: FilterView {
                        model: private_.contentType === FxModuleContent.Type.Filter ? private_.content : null
                        bypassed: private_.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: ScopeView {
                        model: private_.contentType === FxModuleContent.Type.Scope ? private_.content : null
                        bypassed: private_.bypassed
                    }
                }

                BusyLoader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    sourceComponent: SpectrumView {
                        model: private_.contentType === FxModuleContent.Type.Spectrum ? private_.content : null
                        bypassed: private_.bypassed
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
