/*
    Copyright (C) 2012  Dan Vratil <dvratil@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

import QtQuick 2.1
import org.kde.kirigami 2.4 as Kirigami

Rectangle {
    id: root;

    property string outputName;
    property string modeName;

    color: theme.backgroundColor
    border {
        color: theme.textColor
        width: Kirigami.Units.smallSpacing * 1.5
    }
    radius: Kirigami.Units.smallSpacing * 2
    opacity: 0.9

    width: childrenRect.width + 2 * childrenRect.x
    height: childrenRect.height + 2 * childrenRect.y

    Kirigami.Label {
        id: displayName
        x: units.largeSpacing * 2
        y: units.largeSpacing
        font.pointSize: theme.defaultFont.pointSize * 3
        text: root.outputName;
        wrapMode: Text.WordWrap;
        horizontalAlignment: Text.AlignHCenter;
    }

    Kirigami.Label {
        id: modeLabel;
        anchors {
            horizontalCenter: displayName.horizontalCenter
            top: displayName.bottom
        }
        text: root.modeName;
        horizontalAlignment: Text.AlignHCenter;
    }
}
