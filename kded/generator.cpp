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
