/*
    SPDX-FileCopyrightText: 2016 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2022 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "osdmanager.h"
#include "osd.h"
#include "osdserviceadaptor.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QQmlEngine>
#include <disman/config.h>
#include <disman/getconfigoperation.h>
#include <disman/output.h>

namespace KDisplay
{
OsdManager::OsdManager(QObject* parent)
    : QObject(parent)
    , m_cleanupTimer(new QTimer(this))
{
    qmlRegisterUncreatableType<KDisplay::OsdAction>(
        "org.kwinft.kdisplay", 1, 0, "OsdAction", QStringLiteral("Can't create OsdAction"));
    new OsdServiceAdaptor(this);

    // free up memory when the osd hasn't been used for more than 1 minute
    m_cleanupTimer->setInterval(60000);
    m_cleanupTimer->setSingleShot(true);
    connect(m_cleanupTimer, &QTimer::timeout, this, [this]() { quit(); });
    QDBusConnection::sessionBus().registerObject(
        QStringLiteral("/org/kwinft/kdisplay/osdService"), this, QDBusConnection::ExportAdaptors);
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.kwinft.kdisplay.osdService"));
}

void OsdManager::hideOsd()
{
    // Let QML engine finish execution of signal handlers, if any.
    QTimer::singleShot(0, this, &OsdManager::quit);
}

void OsdManager::quit()
{
    qDeleteAll(m_osds);
    m_osds.clear();
    qApp->quit();
}

OsdManager::~OsdManager() = default;

OsdAction::Action OsdManager::showActionSelector()
{
    setDelayedReply(true);

    connect(new Disman::GetConfigOperation(),
            &Disman::GetConfigOperation::finished,
            this,
            [this, message = message()](auto const op) {
                if (op->has_error()) {
                    qWarning() << op->error_string();
                    auto error = message.createErrorReply(
                        QDBusError::Failed,
                        QStringLiteral("Failed to get current output configuration"));
                    QDBusConnection::sessionBus().send(error);
                    return;
                }

                // Show selector on at most one of the enabled screens
                auto const outputs = op->config()->outputs();
                auto primary_output = op->config()->primary_output();
                Disman::OutputPtr osdOutput;
                for (auto const& [id, output] : outputs) {
                    if (!output->enabled() || !output->commanded_mode()) {
                        continue;
                    }

                    // Prefer laptop screen
                    if (output->type() == Disman::Output::Panel) {
                        osdOutput = output;
                        break;
                    }

                    // Fallback to primary
                    if (output == primary_output) {
                        osdOutput = output;
                        break;
                    }
                }

                // no laptop or primary screen, just take the first usable one
                if (!osdOutput) {
                    for (auto const& [id, output] : outputs) {
                        if (output->enabled() && output->commanded_mode()) {
                            osdOutput = output;
                            break;
                        }
                    }
                }

                if (!osdOutput) {
                    auto error = message.createErrorReply(QDBusError::Failed,
                                                          QStringLiteral("No enabled output"));
                    QDBusConnection::sessionBus().send(error);
                    return;
                }

                KDisplay::Osd* osd = nullptr;
                if (m_osds.contains(osdOutput->name())) {
                    osd = m_osds.value(osdOutput->name());
                } else {
                    osd = new KDisplay::Osd(osdOutput, this);
                    m_osds.insert(osdOutput->name(), osd);
                    connect(osd,
                            &Osd::osdActionSelected,
                            this,
                            [this, message](OsdAction::Action action) {
                                auto reply = message.createReply(action);
                                QDBusConnection::sessionBus().send(reply);

                                hideOsd();
                            });
                }

                osd->showActionSelector();
                connect(m_cleanupTimer, &QTimer::timeout, this, [message] {
                    auto reply = message.createReply(OsdAction::NoAction);
                    QDBusConnection::sessionBus().send(reply);
                });
                m_cleanupTimer->start();
            });
    return OsdAction::NoAction;
}

}
