/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "osdaction.h"

#include <disman/types.h>

namespace Generator
{

Disman::ConfigPtr displaySwitch(OsdAction::Action action, Disman::ConfigPtr const& config);

}
