/*************************************************************************************
 *  Copyright (C) 2012 by Alejandro Fiestas Olivares <afiestas@kde.org>              *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/
#include "generator.h"
#include "device.h"
#include "kdisplay_daemon_debug.h"
#include <QRect>

#include <disman/config.h>
#include <disman/generator.h>

#include <algorithm>

#if defined(QT_NO_DEBUG)
#define ASSERT_OUTPUTS(outputs)
#else
#define ASSERT_OUTPUTS(outputs)                                                                    \
    while (true) {                                                                                 \
        assert(!outputs.isEmpty());                                                                \
        for (Disman::OutputPtr const& output : outputs) {                                          \
            assert(output);                                                                        \
            assert(output->id());                                                                  \
            assert(output->auto_mode());                                                           \
        }                                                                                          \
        break;                                                                                     \
    }
#endif

Generator* Generator::instance = nullptr;

Generator* Generator::self()
{
    if (!Generator::instance) {
        Generator::instance = new Generator();
    }
    return Generator::instance;
}

Generator::Generator()
    : QObject()
    , m_forceLaptop(false)
    , m_forceLidClosed(false)
    , m_forceNotLaptop(false)
    , m_forceDocked(false)
{
    connect(Device::self(), &Device::ready, this, &Generator::ready);
}

void Generator::destroy()
{
    delete Generator::instance;
    Generator::instance = nullptr;
}

void Generator::setCurrentConfig(const Disman::ConfigPtr& currentConfig)
{
    m_currentConfig = currentConfig;
}

Disman::ConfigPtr Generator::idealConfig(Disman::ConfigPtr const& config)
{
    Q_ASSERT(config);

    if (!isLaptop()) {
        qCDebug(KDISPLAY_KDED) << "Not a laptop, using optimal config provided by Disman.";
        return config;
    }
    if (config->outputs().isEmpty()) {
        return config;
    }
    return laptop(config);
}

Disman::ConfigPtr Generator::laptop(Disman::ConfigPtr const& config)
{
    Disman::Generator generator(config);
    generator.set_derived();

    auto outputs = generator.config()->outputs();
    auto embedded = generator.embedded();

    /* Apparently older laptops use "VGA-*" as embedded output ID, so embedded()
     * will fail, because it looks only for modern "LVDS", "EDP", etc. If we
     * fail to detect which output is embedded, just use the one  with the lowest
     * ID. It's a wild guess, but I think it's highly probable that it will work.
     * See bug #318907 for further reference. -- dvratil
     */
    if (!embedded) {
        auto keys = outputs.keys();
        std::sort(keys.begin(), keys.end());
        embedded = outputs.value(keys.first());
    }

    if (outputs.size() == 1) {
        qCWarning(KDISPLAY_KDED) << "No external outputs found, keeping current config.";
        return nullptr;
    }

    if (embedded->modes().isEmpty()) {
        qCWarning(KDISPLAY_KDED) << "Embedded output" << embedded
                                 << "has no modes, keeping current config.";
        return nullptr;
    }

    bool success;
    if (isLidClosed()) {
        embedded->setPrimary(false);
        embedded->setEnabled(false);

        Disman::OutputPtr output_to_enable;
        int max_area = 0;
        for (auto output : outputs) {
            // Enable at least one other output.
            if (output->id() == embedded->id()) {
                continue;
            }
            if (output->isEnabled()) {
                output_to_enable = output;
                break;
            }
            auto const size = output->auto_mode()->size();
            auto area = size.width() * size.height();
            if (area > max_area) {
                output_to_enable = output;
                max_area = area;
            }
        }
        output_to_enable->setEnabled(true);

        if (outputs.count() == 2) {
            qCDebug(KDISPLAY_KDED) << "With lid closed and one other display.";
            success = generator.optimize();
        } else {
            qCDebug(KDISPLAY_KDED) << "With closed lid and more than one other display.";
            success = generator.extend(Disman::Generator::Extend_direction::right);
        }

    } else {
        qCDebug(KDISPLAY_KDED) << "With open lid.";

        if (isDocked()) {
            qCDebug(KDISPLAY_KDED) << "Laptop is docked.";
            Disman::OutputList exclude;
            exclude[embedded->id()] = embedded;
            auto primary = generator.primary(exclude);
            if (!primary) {
                primary = generator.biggest(exclude);
            }
            assert(primary);
            primary->setEnabled(true);
            primary->setPrimary(true);
            success = generator.extend(Disman::Generator::Extend_direction::right);
        } else {
            // If lid is open, laptop screen should be primary.
            success = generator.extend(embedded, Disman::Generator::Extend_direction::right);
        }
    }
    if (!success) {
        return nullptr;
    }
    generator.config()->set_origin(Disman::Config::Origin::generated);
    return generator.config();
}

Disman::ConfigPtr Generator::displaySwitch(DisplaySwitchAction action)
{
    qCDebug(KDISPLAY_KDED) << "Display Switch";

    Disman::Generator generator(m_currentConfig);

    auto config = generator.config();
    Q_ASSERT(config);

    auto connectedOutputs = config->outputs();

    // There's not much else we can do with only one output
    if (connectedOutputs.count() < 2) {
        return config;
    }

    // We cannot try all possible combinations with two and more outputs
    if (connectedOutputs.count() > 2) {
        return m_currentConfig;
    }

    auto success = false;
    switch (action) {
    case Generator::ExtendToLeft: {
        qCDebug(KDISPLAY_KDED) << "Extend to left";
        success = generator.extend(Disman::Generator::Extend_direction::left);
        break;
    }
    case Generator::ExtendToRight: {
        qCDebug(KDISPLAY_KDED) << "Extend to right";
        success = generator.extend(Disman::Generator::Extend_direction::right);
        break;
    }
    case Generator::TurnOffEmbedded: {
        qCDebug(KDISPLAY_KDED) << "Turn off embedded (laptop)";
        auto embedded = generator.embedded();
        if (embedded) {
            embedded->setEnabled(false);
            success = generator.optimize();
        }
        break;
    }
    case Generator::TurnOffExternal: {
        qCDebug(KDISPLAY_KDED) << "Turn off external screen";
        // TODO: Why would a user want to do that?
        qCWarning(KDISPLAY_KDED)
            << "Weird option to turn off external was selected, just do nothing instead.";
        break;
    }
    case Generator::Clone: {
        success = generator.replicate();
        break;
    }
    case Generator::None: // just return config
        break;
    }

    if (!success) {
        return nullptr;
    }
    generator.config()->set_origin(Disman::Config::Origin::interactive);
    return generator.config();
}

bool Generator::isLaptop() const
{
    if (m_forceLaptop) {
        return true;
    }
    if (m_forceNotLaptop) {
        return false;
    }

    return Device::self()->isLaptop();
}

bool Generator::isLidClosed() const
{
    if (m_forceLidClosed) {
        return true;
    }
    if (m_forceNotLaptop) {
        return false;
    }

    return Device::self()->isLidClosed();
}

bool Generator::isDocked() const
{
    if (m_forceDocked) {
        return true;
    }

    return Device::self()->isDocked();
}

void Generator::setForceLaptop(bool force)
{
    m_forceLaptop = force;
}

void Generator::setForceLidClosed(bool force)
{
    m_forceLidClosed = force;
}

void Generator::setForceDocked(bool force)
{
    m_forceDocked = force;
}

void Generator::setForceNotLaptop(bool force)
{
    m_forceNotLaptop = force;
}
