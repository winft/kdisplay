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

#ifndef KDED_GENERATOR_H
#define KDED_GENERATOR_H

#include <QObject>

#include <disman/output.h>
#include <disman/mode.h>

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

        void setCurrentConfig(const Disman::ConfigPtr &currentConfig);

        Disman::ConfigPtr idealConfig(const Disman::ConfigPtr &currentConfig);
        Disman::ConfigPtr displaySwitch(DisplaySwitchAction iteration);

        void setForceLaptop(bool force);
        void setForceLidClosed(bool force);
        void setForceDocked(bool force);
        void setForceNotLaptop(bool force);

        static Disman::ModePtr biggestMode(const Disman::ModeList &modes);

    Q_SIGNALS:
        void ready();

    private:
        explicit Generator();
        ~Generator() override;

        Disman::ConfigPtr fallbackIfNeeded(const Disman::ConfigPtr &config);

        void cloneScreens(Disman::OutputList &connectedOutputs);
        void laptop(Disman::OutputList &connectedOutputs);
        void singleOutput(Disman::OutputList &connectedOutputs);
        void extendToRight(Disman::OutputList &connectedOutputs);

        Disman::ModePtr bestModeForSize(const Disman::ModeList& modes, const QSize &size);
        Disman::ModePtr bestModeForOutput(const Disman::OutputPtr &output);
        qreal bestScaleForOutput(const Disman::OutputPtr &output);

        Disman::OutputPtr biggestOutput(const Disman::OutputList &connectedOutputs);
        Disman::OutputPtr embeddedOutput(const Disman::OutputList &connectedOutputs);
        void disableAllDisconnectedOutputs(const Disman::OutputList &connectedOutputs);

        bool isLaptop() const;
        bool isLidClosed() const;
        bool isDocked() const;

        bool m_forceLaptop;
        bool m_forceLidClosed;
        bool m_forceNotLaptop;
        bool m_forceDocked;

        Disman::ConfigPtr m_currentConfig;

        static Generator* instance;
};

#endif //KDED_GENERATOR_H
