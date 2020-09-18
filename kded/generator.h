/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <disman/types.h>

namespace Generator
{

enum class Action {
    None = 0,
    Clone = 1,
    ExtendToLeft = 2,
    TurnOffEmbedded = 3,
    TurnOffExternal = 4,
    ExtendToRight = 5,
};

Disman::ConfigPtr displaySwitch(Action iteration, Disman::ConfigPtr const& config);

}
