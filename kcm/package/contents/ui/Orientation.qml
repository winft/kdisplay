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
import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3 as Controls
import org.kde.kirigami 2.4 as Kirigami

ColumnLayout {
    Kirigami.FormData.label: i18n("Orientation:")
    Kirigami.FormData.buddyFor: auto_rotate_switch
    spacing: Kirigami.Units.smallSpacing

    ColumnLayout {
        id: autoRotateColumn

        // TODO: Make this dependend on tablet mode being available
        enabled: kcm.orientationSensorAvailable && element.internal
        visible: kcm.autoRotationSupported

        Controls.Switch {
            id: auto_rotate_switch
            text: i18n("Auto")
            checked: enabled && element.autoRotate
            onToggled: element.autoRotate = checked
        }

        Controls.Switch {
            text: i18n("Only when in tablet mode.")
            visible: auto_rotate_switch.checked
            checked: element.autoRotateOnlyInTabletMode
            onToggled: element.autoRotateOnlyInTabletMode = checked
        }
    }

    RowLayout {
       id: orientation
       visible: !auto_rotate_switch.checked || !kcm.autoRotationSupported

       Controls.ButtonGroup {
           buttons: orientation.children
       }

       RotationButton {
           value: 0
       }
       RotationButton {
           value: 90
       }
       RotationButton {
           value: 180
       }
       RotationButton {
           value: 270
       }
    }
}
