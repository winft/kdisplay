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
    if (config->outputs().empty()) {
        return config;
    }
    return laptop(config);
}

Disman::ConfigPtr Generator::laptop(Disman::ConfigPtr const& config)
{
    Disman::Generator generator(config);
    generator.set_derived();
    auto generated_config = generator.config();

    auto outputs = generated_config->outputs();
    auto embedded = generator.embedded();

    /* Apparently older laptops use "VGA-*" as embedded output ID, so embedded()
     * will fail, because it looks only for modern "LVDS", "EDP", etc. If we
     * fail to detect which output is embedded, just use the one  with the lowest
     * ID. It's a wild guess, but I think it's highly probable that it will work.
     * See bug #318907 for further reference. -- dvratil
     */
    if (!embedded) {
        std::vector<int> keys;
        for (auto const& [key, out] : outputs) {
            keys.push_back(key);
        }
        std::sort(keys.begin(), keys.end());
        embedded = outputs.at(*(keys.begin()));
    }

    if (outputs.size() == 1) {
        qCWarning(KDISPLAY_KDED) << "No external outputs found, keeping current config.";
        return nullptr;
    }

    if (embedded->modes().empty()) {
        qCWarning(KDISPLAY_KDED) << "Embedded output" << embedded
                                 << "has no modes, keeping current config.";
        return nullptr;
    }

    bool success;
    if (isLidClosed()) {
        if (generated_config->primary_output() == embedded) {
            generated_config->set_primary_output(nullptr);
        }
        embedded->set_enabled(false);

        Disman::OutputPtr output_to_enable;
        int max_area = 0;
        for (auto const& [key, output] : outputs) {
            // Enable at least one other output.
            if (output->id() == embedded->id()) {
                continue;
            }
            if (output->enabled()) {
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
        output_to_enable->set_enabled(true);

        if (outputs.size() == 2) {
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
            Disman::OutputMap exclude;
            exclude[embedded->id()] = embedded;
            auto primary = generator.primary(exclude);
            if (!primary) {
                primary = generator.biggest(exclude);
            }
            assert(primary);
            primary->set_enabled(true);

            if (generated_config->supported_features() & Disman::Config::Feature::PrimaryDisplay) {
                generated_config->set_primary_output(primary);
            }

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

    auto const outputs_cnt = m_currentConfig->outputs().size();
    if (outputs_cnt < 2) {
        qCDebug(KDISPLAY_KDED) << "Only one output connected. Display Switch not applicable.";
        return m_currentConfig;
    }
    if (outputs_cnt > 2) {
        qCDebug(KDISPLAY_KDED) << "More than two outputs connected. Display Switch not applicable.";
        return m_currentConfig;
    }

    Disman::Generator generator(m_currentConfig);

    auto config = generator.config();
    Q_ASSERT(config);

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
            embedded->set_enabled(false);
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
