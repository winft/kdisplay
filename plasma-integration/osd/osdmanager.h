/*
    SPDX-FileCopyrightText: 2016 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "osdaction.h"

#include <QDBusContext>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>

namespace KDisplay
{

class Osd;

class OsdManager : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    OsdManager(QObject* parent = nullptr);
    ~OsdManager() override;

public Q_SLOTS:
    void hideOsd();
    OsdAction::Action showActionSelector();

private:
    void quit();
    QMap<std::string, KDisplay::Osd*> m_osds;
    QTimer* m_cleanupTimer;
};

} // ns
