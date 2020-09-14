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
#pragma once

#include <QObject>

#include <disman/mode.h>
#include <disman/output.h>

namespace Disman
{
class Config;
}

class Generator : public QObject
{
    Q_OBJECT
public:
    enum DisplaySwitchAction {
        None = 0,
        Clone = 1,
        ExtendToLeft = 2,
        TurnOffEmbedded = 3,
        TurnOffExternal = 4,
        ExtendToRight = 5,
    };

    static Generator* self();
    static void destroy();

    void setCurrentConfig(const Disman::ConfigPtr& currentConfig);

    Disman::ConfigPtr displaySwitch(DisplaySwitchAction iteration);

private:
    Generator();

    Disman::ConfigPtr m_currentConfig;

    static Generator* instance;
};
