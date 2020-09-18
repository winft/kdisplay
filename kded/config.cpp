/********************************************************************
Copyright 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
Copyright 2019 Roman Gilg <subdiff@gmail.com>

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
#include "config.h"

#include "kdisplay_daemon_debug.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QRect>
#include <QStandardPaths>

#include <disman/config.h>
#include <disman/output.h>

Config::Config(Disman::ConfigPtr config, QObject* parent)
    : QObject(parent)
    , m_data(config)
{
}

bool Config::autoRotationRequested() const
{
    for (auto const& [key, output] : m_data->outputs()) {
        if (output->auto_rotate()) {
            // We say auto rotation is requested when at least one output does.
            return true;
        }
    }
    return false;
}

Disman::Output::Rotation orientationToRotation(QOrientationReading::Orientation orientation,
                                               Disman::Output::Rotation fallback)
{
    using Orientation = QOrientationReading::Orientation;

    switch (orientation) {
    case Orientation::TopUp:
        return Disman::Output::Rotation::None;
    case Orientation::TopDown:
        return Disman::Output::Rotation::Inverted;
    case Orientation::LeftUp:
        return Disman::Output::Rotation::Right;
    case Orientation::RightUp:
        return Disman::Output::Rotation::Left;
    case Orientation::Undefined:
    case Orientation::FaceUp:
    case Orientation::FaceDown:
        return fallback;
    default:
        Q_UNREACHABLE();
    }
}

bool updateOrientation(Disman::OutputPtr& output, QOrientationReading::Orientation orientation)
{
    if (output->type() != Disman::Output::Type::Panel) {
        return false;
    }
    const auto currentRotation = output->rotation();
    const auto rotation = orientationToRotation(orientation, currentRotation);
    if (rotation == currentRotation) {
        return true;
    }
    output->set_rotation(rotation);
    return true;
}

void Config::setDeviceOrientation(QOrientationReading::Orientation orientation)
{
    for (auto& [key, output] : m_data->outputs()) {
        if (!output->auto_rotate()) {
            continue;
        }
        auto finalOrientation = orientation;
        if (output->auto_rotate_only_in_tablet_mode() && !m_data->tablet_mode_engaged()) {
            finalOrientation = QOrientationReading::Orientation::TopUp;
        }
        if (updateOrientation(output, finalOrientation)) {
            // TODO: call Layouter to find fitting positions for other outputs again
            return;
        }
    }
}

bool Config::getAutoRotate() const
{
    const auto outputs = m_data->outputs();
    return std::all_of(outputs.cbegin(), outputs.cend(), [this](auto const& output) {
        if (output.second->type() != Disman::Output::Type::Panel) {
            return true;
        }
        return output.second->auto_rotate();
    });
}

void Config::setAutoRotate(bool value)
{
    for (auto const& [key, output] : m_data->outputs()) {
        if (output->type() == Disman::Output::Type::Panel) {
            // For now we only set it for panel-type outputs.
            output->set_auto_rotate(value);
        }
    }
}

void Config::log()
{
    if (!m_data) {
        return;
    }
    const auto outputs = m_data->outputs();
    for (const auto& o : outputs) {
        qCDebug(KDISPLAY_KDED) << o;
    }
}
