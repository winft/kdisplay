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
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.kirigami 2.20 as Kirigami

ColumnLayout {
    spacing: 0

    states: [
        State {
            // only makes sense to offer screen layout setup if there's more than one screen connected
            when: Plasmoid.connectedOutputCount < 2

            PropertyChanges {
                target: screenLayoutRow
                enabled: false
            }
            PropertyChanges {
                target: noScreenLabel
                visible: true
            }
        }
    ]

    PlasmaExtras.Heading {
        Layout.fillWidth: true
        level: 3
        text: i18n("Display Layout")
    }

    // Screen layout selector section
    Row {
        id: screenLayoutRow
        readonly property int buttonSize: Math.floor((width - spacing * (screenLayoutRepeater.count - 1)) / screenLayoutRepeater.count)
        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Repeater {
            id: screenLayoutRepeater
            model: root.screenLayouts

            PlasmaComponents3.Button {
                width: screenLayoutRow.buttonSize
                height: width
                onClicked: Plasmoid.applyLayoutPreset(modelData.action)

                Accessible.name: modelData.label
                PlasmaComponents3.ToolTip { text: modelData.label }

                // HACK otherwise the icon won't expand to full button size
                PlasmaCore.IconItem {
                    anchors.centerIn: parent
                    width: height
                    // FIXME use proper FrameSvg margins and stuff
                    height: parent.height - Kirigami.Units.smallSpacing
                    source: modelData.iconName
                    active: parent.hovered
                }
            }
        }
    }

    PlasmaExtras.DescriptiveLabel {
        id: noScreenLabel
        Layout.fillWidth: true
        Layout.maximumWidth: Math.min(Kirigami.Units.gridUnit * 20, implicitWidth)
        wrapMode: Text.Wrap
        text: i18n("You can only apply a different display layout when there is more than one display device plugged in.")
        font.pointSize: theme.smallestFont.pointSize
        visible: false
    }
}
