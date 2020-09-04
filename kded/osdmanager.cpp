/*
 *  Copyright 2016 Sebastian Kügler <sebas@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "osdmanager.h"
#include "kdisplay_daemon_debug.h"
#include "osd.h"

#include <disman/config.h>
#include <disman/getconfigoperation.h>
#include <disman/output.h>

#include <QDBusConnection>

#include <QQmlEngine>

namespace Disman
{

class OsdActionImpl : public OsdAction
{
    Q_OBJECT
public:
    OsdActionImpl(QObject* parent = nullptr)
        : OsdAction(parent)
    {
    }

    void setOsd(Osd* osd)
    {
        connect(osd, &Osd::osdActionSelected, this, [this](Action action) {
            Q_EMIT selected(action);
            deleteLater();
        });
    }
};

OsdManager::OsdManager(QObject* parent)
    : QObject(parent)
    , m_cleanupTimer(new QTimer(this))
{
    qmlRegisterSingletonType<Disman::OsdAction>(
        "org.kwinft.kdisplay", 1, 0, "OsdAction", [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new Disman::OsdAction();
        });

    // free up memory when the osd hasn't been used for more than 1 minute
    m_cleanupTimer->setInterval(60000);
    m_cleanupTimer->setSingleShot(true);
    connect(m_cleanupTimer, &QTimer::timeout, this, [this]() { hideOsd(); });
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.kwinft.kdisplay.osdService"));
    if (!QDBusConnection::sessionBus().registerObject(
            QStringLiteral("/org/kwinft/kdisplay/osdService"),
            this,
            QDBusConnection::ExportAllSlots)) {
        qCWarning(KDISPLAY_KDED) << "Failed to registerObject";
    }
}

void OsdManager::hideOsd()
{
    qDeleteAll(m_osds);
    m_osds.clear();
}

OsdManager::~OsdManager()
{
}

void OsdManager::showOutputIdentifiers()
{
    connect(new Disman::GetConfigOperation(),
            &Disman::GetConfigOperation::finished,
            this,
            &OsdManager::slotIdentifyOutputs);
}

void OsdManager::slotIdentifyOutputs(Disman::ConfigOperation* op)
{
    if (op->hasError()) {
        return;
    }

    const Disman::ConfigPtr config = qobject_cast<Disman::GetConfigOperation*>(op)->config();

    Q_FOREACH (const Disman::OutputPtr& output, config->outputs()) {
        if (!output->isEnabled() || !output->auto_mode()) {
            continue;
        }
        auto osd = m_osds.value(QString::fromStdString(output->name()));
        if (!osd) {
            osd = new Disman::Osd(output, this);
            m_osds.insert(QString::fromStdString(output->name()), osd);
        }
        osd->showOutputIdentifier(output);
    }
    m_cleanupTimer->start();
}

void OsdManager::showOsd(const QString& icon, const QString& text)
{
    hideOsd();

    connect(new Disman::GetConfigOperation(),
            &Disman::GetConfigOperation::finished,
            this,
            [this, icon, text](Disman::ConfigOperation* op) {
                if (op->hasError()) {
                    return;
                }

                const Disman::ConfigPtr config
                    = qobject_cast<Disman::GetConfigOperation*>(op)->config();

                Q_FOREACH (const Disman::OutputPtr& output, config->outputs()) {
                    if (!output->isEnabled() || !output->auto_mode()) {
                        continue;
                    }
                    auto osd = m_osds.value(QString::fromStdString(output->name()));
                    if (!osd) {
                        osd = new Disman::Osd(output, this);
                        m_osds.insert(QString::fromStdString(output->name()), osd);
                    }
                    osd->showGenericOsd(icon, text);
                }
                m_cleanupTimer->start();
            });
}

OsdAction* OsdManager::showActionSelector()
{
    hideOsd();

    OsdActionImpl* action = new OsdActionImpl(this);
    connect(action, &OsdActionImpl::selected, this, [this]() {
        for (auto osd : qAsConst(m_osds)) {
            osd->hideOsd();
        }
    });
    connect(new Disman::GetConfigOperation(),
            &Disman::GetConfigOperation::finished,
            this,
            [this, action](const Disman::ConfigOperation* op) {
                if (op->hasError()) {
                    qCWarning(KDISPLAY_KDED) << op->errorString();
                    return;
                }

                // Show selector on all enabled screens
                const auto outputs = op->config()->outputs();
                Disman::OutputPtr osdOutput;
                for (const auto& output : outputs) {
                    if (!output->isEnabled() || !output->auto_mode()) {
                        continue;
                    }

                    // Prefer laptop screen
                    if (output->type() == Disman::Output::Panel) {
                        osdOutput = output;
                        break;
                    }

                    // Fallback to primary
                    if (output->isPrimary()) {
                        osdOutput = output;
                        break;
                    }
                }
                // no laptop or primary screen, just take the first usable one
                if (!osdOutput) {
                    for (const auto& output : outputs) {
                        if (output->isEnabled() && output->auto_mode()) {
                            osdOutput = output;
                            break;
                        }
                    }
                }

                if (!osdOutput) {
                    // huh!?
                    return;
                }

                Disman::Osd* osd = nullptr;
                auto const name = QString::fromStdString(osdOutput->name());
                if (m_osds.contains(name)) {
                    osd = m_osds.value(name);
                } else {
                    osd = new Disman::Osd(osdOutput, this);
                    m_osds.insert(name, osd);
                }
                action->setOsd(osd);
                osd->showActionSelector();
                m_cleanupTimer->start();
            });

    return action;
}

}

#include "osdmanager.moc"
