/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2018-2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KSCREEN_DAEMON_H
#define KSCREEN_DAEMON_H

#include "osdaction.h"

#include <disman/config.h>

#include <kdedmodule.h>

#include <QVariant>

class OrientationSensor;

namespace Disman
{
class OsdManager;
}

class KDisplayDaemon : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kwinft.kdisplay")

public:
    KDisplayDaemon(QObject* parent, const QList<QVariant>&);

public Q_SLOTS:
    // DBus
    void applyLayoutPreset(const QString& presetName);
    bool getAutoRotate();
    void setAutoRotate(bool value);

Q_SIGNALS:
    // DBus
    void outputConnected(const QString& outputName);
    void unknownOutputConnected(const QString& outputName);

private:
    void init();

    void applyConfig();
    void configChanged();
    void displayButton();
    void setMonitorForChanges(bool enabled);

    void showOutputIdentifier();
    void applyOsdAction(Disman::OsdAction::Action action);

    void doApplyConfig(Disman::ConfigPtr const& config);
    void refreshConfig();

    void monitorConnectedChange();
    void showOsd(const QString& icon, const QString& text);

    void updateOrientation();

    Disman::ConfigPtr m_monitoredConfig;
    bool m_monitoring;
    bool m_configDirty = true;
    Disman::OsdManager* m_osdManager;
    OrientationSensor* m_orientationSensor;
    bool m_startingUp = true;
};

#endif /*KSCREEN_DAEMON_H*/
