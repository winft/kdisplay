/*
 * Copyright (c) 2018 Kai Uwe Broulik <kde@broulik.de>
 *                    Work sponsored by the LiMux project of
 *                    the city of Munich.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.plasma5support 2.0 as P5Support
import org.kde.kquickcontrolsaddons 2.0

import org.kwinft.private.kdisplay 1.0

PlasmoidItem {
    id: root

    // Only show if there's screen layouts available or the user enabled presentation mode
    Plasmoid.status: presentationModeEnabled || (isLaptop && Plasmoid.connectedOutputCount > 1) ? PlasmaCore.Types.ActiveStatus : PlasmaCore.Types.PassiveStatus
    toolTipSubText: presentationModeEnabled ? i18n("Presentation mode is enabled") : ""

    readonly property string kcmName: "kcm_kdisplay"
    readonly property bool kcmAllowed: KCMShell.authorize(kcmName + ".desktop").length > 0

    readonly property bool presentationModeEnabled: presentationModeCookie > 0
    property int presentationModeCookie: -1

    readonly property var screenLayouts: {
        var layouts = OsdAction.actionOrder().filter(function (layout) {
            // We don't want the "No action" item in the plasmoid
            return layout !== OsdAction.NoAction;
        });

        layouts.map(function (layout) {
            return {
                iconName: OsdAction.actionIconName(layout),
                label: OsdAction.actionLabel(layout),
                action: layout
            }
        });
    }

    function action_configure() {
        KCMShell.openSystemSettings(kcmName);
    }

    P5Support.DataSource {
        id: pmSource
        engine: "powermanagement"
        connectedSources: ["PowerDevil", "Inhibitions"]

        onSourceAdded: source => {
            disconnectSource(source);
            connectSource(source);
        }
        onSourceRemoved: source => {
            disconnectSource(source);
        }

        readonly property var inhibitions: {
            var inhibitions = [];

            var data = pmSource.data.Inhibitions;
            if (data) {
                for (var key in data) {
                    if (key === "plasmashell" || key === "plasmoidviewer") { // ignore our own inhibition
                        continue;
                    }

                    inhibitions.push(data[key]);
                }
            }

            return inhibitions;
        }
    }

    Component.onCompleted: {
        if (kcmAllowed) {
            Plasmoid.removeAction("configure");
            Plasmoid.setAction("configure", i18n("Configure Display Settings…"), "preferences-desktop-display")
        }
    }

    fullRepresentation: ColumnLayout {
        spacing: 0
        Layout.preferredWidth: units.gridUnit * 15

        ScreenLayoutSelection {
            Layout.leftMargin: units.smallSpacing
            Layout.fillWidth: true
        }

        PresentationModeItem {
            Layout.fillWidth: true
            Layout.topMargin: units.largeSpacing
            Layout.leftMargin: units.smallSpacing
        }

        // compact the layout
        Item {
            Layout.fillHeight: true
        }
    }
}
