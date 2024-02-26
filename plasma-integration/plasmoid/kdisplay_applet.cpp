/*
 * Copyright (c) 2018 Kai Uwe Broulik <kde@broulik.de>
 *                    Work sponsored by the LiMux project of
 *                    the city of Munich.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "kdisplay_applet.h"

#include "../osd/osdaction.h"

#include <QMetaEnum>
#include <QQmlEngine> // for qmlRegisterType

#include <QDBusConnection>
#include <QDBusMessage>

#include <disman/config.h>
#include <disman/configmonitor.h>
#include <disman/getconfigoperation.h>
#include <disman/output.h>

#include <algorithm>

KDisplayApplet::KDisplayApplet(QObject* parent,
                               const KPluginMetaData& data,
                               const QVariantList& args)
    : Plasma::Applet(parent, data, args)
{
    qmlRegisterUncreatableType<KDisplay::OsdAction>(
        "org.kwinft.private.kdisplay", 1, 0, "OsdAction", QStringLiteral("Can't create OsdAction"));
}

KDisplayApplet::~KDisplayApplet() = default;

void KDisplayApplet::init()
{
    connect(new Disman::GetConfigOperation,
            &Disman::ConfigOperation::finished,
            this,
            [this](Disman::ConfigOperation* op) {
                m_screenConfiguration = qobject_cast<Disman::GetConfigOperation*>(op)->config();

                Disman::ConfigMonitor::instance()->add_config(m_screenConfiguration);
                connect(Disman::ConfigMonitor::instance(),
                        &Disman::ConfigMonitor::configuration_changed,
                        this,
                        &KDisplayApplet::checkOutputs);

                checkOutputs();
            });
}

int KDisplayApplet::connectedOutputCount() const
{
    return m_connectedOutputCount;
}

void KDisplayApplet::applyLayoutPreset(Action action)
{
    auto const actionEnum = QMetaEnum::fromType<KDisplay::OsdAction::Action>();
    Q_ASSERT(actionEnum.isValid());

    const QString presetName = QString::fromLatin1(actionEnum.valueToKey(action));
    if (presetName.isEmpty()) {
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kded6"),
                                                      QStringLiteral("/modules/kdisplay"),
                                                      QStringLiteral("org.kwinft.kdisplay"),
                                                      QStringLiteral("applyLayoutPreset"));

    msg.setArguments({presetName});

    QDBusConnection::sessionBus().call(msg, QDBus::NoBlock);
}

void KDisplayApplet::checkOutputs()
{
    if (!m_screenConfiguration) {
        return;
    }

    const int oldConnectedOutputCount = m_connectedOutputCount;

    const auto outputs = m_screenConfiguration->outputs();
    m_connectedOutputCount = outputs.size();

    if (m_connectedOutputCount != oldConnectedOutputCount) {
        Q_EMIT connectedOutputCountChanged();
    }
}

K_PLUGIN_CLASS(KDisplayApplet)

#include "kdisplay_applet.moc"
