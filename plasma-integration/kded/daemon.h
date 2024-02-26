/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2018-2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KSCREEN_DAEMON_H
#define KSCREEN_DAEMON_H

#include "../osd/osdaction.h"

#include <disman/config.h>

#include <kdedmodule.h>

#include <QVariant>

class OrgKwinftKdisplayOsdServiceInterface;

namespace Disman
{
class ConfigOperation;
}

class OrientationSensor;

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

private:
    void init(Disman::ConfigOperation* op);

    void applyConfig();
    void configChanged();
    void displayButton();
    void setMonitorForChanges(bool enabled);

    void show_osd();
    void applyOsdAction(KDisplay::OsdAction::Action action);

    void doApplyConfig(Disman::ConfigPtr const& config);
    void refreshConfig();

    void update_auto_rotate();
    void updateOrientation();

    Disman::ConfigPtr m_monitoredConfig;
    bool m_monitoring;
    bool m_configDirty = true;
    OrgKwinftKdisplayOsdServiceInterface* m_osdServiceInterface;
    OrientationSensor* m_orientationSensor;
    bool m_startingUp = true;
};

#endif /*KSCREEN_DAEMON_H*/
