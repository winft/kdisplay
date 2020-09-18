/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "generator.h"

#include "kdisplay_daemon_debug.h"

#include <disman/config.h>
#include <disman/generator.h>
#include <disman/output.h>

namespace Generator
{

Disman::ConfigPtr displaySwitch(Action action, Disman::ConfigPtr const& config)
{
    qCDebug(KDISPLAY_KDED) << "Display Switch";

    auto const outputs_cnt = config->outputs().size();
    if (outputs_cnt < 2) {
        qCDebug(KDISPLAY_KDED) << "Only one output connected. Display Switch not applicable.";
        return nullptr;
    }
    if (outputs_cnt > 2) {
        qCDebug(KDISPLAY_KDED) << "More than two outputs connected. Display Switch not applicable.";
        return nullptr;
    }

    Disman::Generator generator(config);

    auto success = false;
    switch (action) {
    case Action::ExtendToLeft: {
        qCDebug(KDISPLAY_KDED) << "Extend to left";
        success = generator.extend(Disman::Generator::Extend_direction::left);
        break;
    }
    case Action::ExtendToRight: {
        qCDebug(KDISPLAY_KDED) << "Extend to right";
        success = generator.extend(Disman::Generator::Extend_direction::right);
        break;
    }
    case Action::TurnOffEmbedded: {
        qCDebug(KDISPLAY_KDED) << "Turn off embedded (laptop)";
        auto embedded = generator.embedded();
        if (embedded) {
            embedded->set_enabled(false);
            success = generator.optimize();
        }
        break;
    }
    case Action::TurnOffExternal: {
        qCDebug(KDISPLAY_KDED) << "Turn off external screen";
        // TODO: Why would a user want to do that?
        qCWarning(KDISPLAY_KDED)
            << "Weird option to turn off external was selected, just do nothing instead.";
        break;
    }
    case Action::Clone: {
        success = generator.replicate();
        break;
    }
    case Action::None: // just return config
        success = false;
        break;
    }

    if (!success) {
        return nullptr;
    }
    generator.config()->set_cause(Disman::Config::Cause::interactive);
    return generator.config();
}

}
