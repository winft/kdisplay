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
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.4 as Kirigami

import org.kde.kcm 1.2 as KCM

Kirigami.FormLayout {
    twinFormLayouts: globalSettingsLayout

    property var element: model

    Controls.CheckBox {
       id: enabled_checkbox
       text: i18n("Enabled")
       checked: element.enabled
       onClicked: element.enabled = checked
       visible: kcm.outputModel.rowCount() > 1
    }

    Controls.CheckBox {
       id: primary_checkbox
       text: i18n("Primary")
       checked: element.primary
       onClicked: element.primary = checked
       visible: kcm.primaryOutputSupported && kcm.outputModel.rowCount() > 1
    }

    Item {
        visible: primary_checkbox.visible || enabled_checkbox.visible
        Kirigami.FormData.isSection: false
    }

    RowLayout {
        Layout.fillWidth: true
        // Set the same limit as the device ComboBox
        Layout.maximumWidth: Kirigami.Units.gridUnit * 16

        visible: kcm.perOutputScaling
        Kirigami.FormData.label: i18n("Scale:")

        Controls.Slider {
            id: scaleSlider

            Layout.fillWidth: true
            from: 0.5
            to: 3
            stepSize: 0.25
            live: true
            value: element.scale
            onMoved: element.scale = value
        }
        Controls.SpinBox {
            id: spinbox
            // Because QQC2 SpinBox doesn't natively support decimal step
            // sizes: https://bugreports.qt.io/browse/QTBUG-67349
            property real factor: 20.0
            property real realValue: value / factor

            from : 0.5 * factor
            to : 3.0 * factor
            stepSize: 0.05 * factor
            value: element.scale * factor
            validator: DoubleValidator {
                bottom: Math.min(spinbox.from, spinbox.to) * spinbox.factor
                top:  Math.max(spinbox.from, spinbox.to) * spinbox.factor
            }
            textFromValue: function(value, locale) {
                return i18nc("Global scale factor expressed in percentage",
                             "%1%", parseFloat(value * 1.0 / factor * 100.0));
            }
            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text.replace("%", "")) * factor / 100.0
            }
            onValueModified: element.scale = realValue
        }
    }

    Item {
        Kirigami.FormData.isSection: false
    }

    Orientation {}

    Item {
        Kirigami.FormData.isSection: false
    }

    ColumnLayout {
        Kirigami.FormData.label: i18n("Resolution:")
        Kirigami.FormData.buddyFor: auto_resolution_switch

        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Controls.Switch {
            id: auto_resolution_switch
            text: i18n("Auto")
            checked: element.autoResolution
            onToggled: element.autoResolution = checked
        }

        Controls.ComboBox {
            enabled: !auto_resolution_switch.checked
            model: element.resolutions
            currentIndex: element.resolutionIndex !== undefined ?
                              element.resolutionIndex : -1
            onActivated: element.resolutionIndex = currentIndex
        }
    }

    Item {
        Kirigami.FormData.isSection: false
    }

    ColumnLayout {
        Kirigami.FormData.label: i18n("Refresh rate:")
        Kirigami.FormData.buddyFor: auto_refresh_rate_switch

        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Controls.Switch {
            id: auto_refresh_rate_switch
            text: i18n("Auto")
            checked: element.autoRefreshRate
            onToggled: element.autoRefreshRate = checked
        }

        Controls.ComboBox {
            enabled: !auto_refresh_rate_switch.checked
            Kirigami.FormData.label: i18n("Refresh rate:")
            model: element.refreshRates
            currentIndex: element.refreshRateIndex
            onActivated: element.refreshRateIndex = currentIndex
        }
    }

    Controls.ComboBox {
        Kirigami.FormData.label: i18n("Replica of:")
        model: element.replicationSourceModel
        visible: kcm.outputReplicationSupported && kcm.outputModel && kcm.outputModel.rowCount() > 1

        onModelChanged: enabled = (count > 1);
        onCountChanged: enabled = (count > 1);

        currentIndex: element.replicationSourceIndex
        onActivated: element.replicationSourceIndex = currentIndex
    }
}
