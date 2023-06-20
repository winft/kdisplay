/********************************************************************
Copyright © 2019 Roman Gilg <subdiff@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
import QtQuick 2.15 as QtQuick
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kitemmodels 1.0

import org.kde.kcmutils as KCM
import org.kwinft.private.kcm.kdisplay 1.0 as KDisplay

KCM.SimpleKCM {
    id: root

    property int selectedOutput: 0
    readonly property int topMargins: Kirigami.Units.smallSpacing
    readonly property bool anyMessagesShown: invalidConfigMsg.visible
                                             || errBackendMsg.visible
                                             || errSaveMsg.visible
                                             || scaleMsg.visible
                                             || connectMsg.visible

    implicitWidth: Kirigami.Units.gridUnit * 32
    implicitHeight: Kirigami.Units.gridUnit * 30

    topPadding: anyMessagesShown ? topMargins : 0
    leftPadding: 0
    rightPadding: 0

    // This is to fix Output dragging
    flickable.interactive: Kirigami.Settings.hasTransientTouchInput

    QtQuick.Connections {
        target: kcm
        function onInvalidConfig(reason) {
            if (reason === KDisplay.KCMKDisplay.NoEnabledOutputs) {
                invalidConfigMsg.text = i18nc("@info", "All displays are disabled. Enable at least one.")
            } else if (reason === KDisplay.KCMKDisplay.ConfigHasGaps) {
                invalidConfigMsg.text = i18nc("@info", "Gaps between displays are not supported. Make sure all displays are touching.")
            }
            invalidConfigMsg.visible = true;
        }
        function onErrorOnSave() {
            errSaveMsg.visible = true;
        }
        function onGlobalScaleWritten() {
            scaleMsg.visible = true;
        }
        function onOutputConnect(connected) {
            if (connected) {
                connectMsg.text = i18n("A new output has been added. Settings have been reloaded.");
            } else {
                connectMsg.text = i18n("An output has been removed. Settings have been reloaded.");
            }
            connectMsg.visible = true;
        }
        function onBackendError() {
            errBackendMsg.visible = true;
        }
        function onChanged() {
            invalidConfigMsg.visible = false;
            errSaveMsg.visible = false;
            scaleMsg.visible = false;
        }
    }


    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.InlineMessage {
            // Note1: There is an implicit height binding loop error on
            //        first invocation. Seems to be an issue in Kirigami.
            // Note2: This should maybe go in header component of the KCM,
            //        but there seems to be another issue in Kirigami then
            //        being always hidden. Compare Night Color KCM with
            //        the same issue.
            id: invalidConfigMsg

            Layout.fillWidth: true
            Layout.leftMargin: root.topMargins
            Layout.rightMargin: root.topMargins
            type: Kirigami.MessageType.Error
            showCloseButton: true

        }
        Kirigami.InlineMessage {
            id: errBackendMsg
            Layout.fillWidth: true
            Layout.leftMargin: root.topMargins
            Layout.rightMargin: root.topMargins
            type: Kirigami.MessageType.Error
            text: i18n("No Disman backend found. Please check your Disman installation.")
            visible: false
            showCloseButton: false
        }
        Kirigami.InlineMessage {
            id: errSaveMsg
            Layout.fillWidth: true
            Layout.leftMargin: root.topMargins
            Layout.rightMargin: root.topMargins
            type: Kirigami.MessageType.Error
            text: i18n("Displays could not be saved due to error.")
            visible: false
            showCloseButton: true
        }
        Kirigami.InlineMessage {
            id: scaleMsg
            Layout.fillWidth: true
            Layout.leftMargin: root.topMargins
            Layout.rightMargin: root.topMargins
            type: Kirigami.MessageType.Information
            text: i18n("Global scale changes will come into effect after the system is restarted.")
            visible: false
            showCloseButton: true
            actions: [
                Kirigami.Action {
                    icon.name: "system-reboot"
                    text: i18n("Restart")
                    onTriggered: kcm.requestReboot();
                }
            ]
        }
        Kirigami.InlineMessage {
            id: connectMsg
            Layout.fillWidth: true
            Layout.leftMargin: root.topMargins
            Layout.rightMargin: root.topMargins
            type: Kirigami.MessageType.Information
            visible: false
            showCloseButton: true
        }

        QtQuick.Connections {
            target: kcm
            function onInvalidConfig(reason) {
                if (reason === KDisplay.KCMKDisplay.NoEnabledOutputs) {
                    invalidConfigMsg.text = i18nc("@info", "All displays are disabled. Enable at least one.")
                } else if (reason === KDisplay.KCMKDisplay.ConfigHasGaps) {
                    invalidConfigMsg.text = i18nc("@info", "Gaps between displays are not supported. Make sure all displays are touching.")
                }
                invalidConfigMsg.visible = true;
            }
            function onErrorOnSave() {
                errSaveMsg.visible = true;
            }
            function onGlobalScaleWritten() {
                scaleMsg.visible = true;
            }
            function onOutputConnect(connected) {
                root.selectedOutput = 0;
                if (connected) {
                    connectMsg.text = i18n("A new output has been added. Settings have been reloaded.");
                } else {
                    connectMsg.text = i18n("An output has been removed. Settings have been reloaded.");
                }
                connectMsg.visible = true;
            }
            function onBackendError() {
                errBackendMsg.visible = true;
            }
            function onChanged() {
                invalidConfigMsg.visible = false;
                errSaveMsg.visible = false;
                scaleMsg.visible = false;
            }
        }

        QtQuick.Rectangle {
            Layout.preferredHeight: Math.max(root.height * 0.4, Kirigami.Units.gridUnit * 13)
            Layout.fillWidth: true
            Kirigami.Theme.inherit: false
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            color: Kirigami.Theme.backgroundColor

            Kirigami.Separator {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                visible: root.anyMessagesShown
            }

            Screen {
                id: screen

                anchors.fill: parent
                enabled: kcm.outputModel && kcm.backendReady
                outputs: kcm.outputModel
            }

            Kirigami.Separator {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
            }
        }

        Panel {
            enabled: kcm.outputModel && kcm.backendReady
            Layout.fillWidth: true
        }
    }
}
