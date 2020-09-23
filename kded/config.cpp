/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2019, 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "config.h"

#include "kdisplay_daemon_debug.h"

#include <disman/config.h>
#include <disman/output.h>

Config::Config(Disman::ConfigPtr config)
    : m_data(config)
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
