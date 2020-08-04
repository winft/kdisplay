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

#include <QMetaEnum>
#include <QQmlEngine> // for qmlRegisterType

#include <QDBusConnection>
#include <QDBusMessage>

#include <Disman/Config>
#include <Disman/ConfigMonitor>
#include <Disman/GetConfigOperation>
#include <Disman/Output>

#include "../kded/osdaction.h"

#include <algorithm>

KDisplayApplet::KDisplayApplet(QObject* parent, const QVariantList& data)
    : Plasma::Applet(parent, data)
{
}

KDisplayApplet::~KDisplayApplet() = default;

void KDisplayApplet::init()
{
    qmlRegisterSingletonType<Disman::OsdAction>(
        "org.kwinft.private.kdisplay", 1, 0, "OsdAction", [](QQmlEngine*, QJSEngine*) -> QObject* {
            return new Disman::OsdAction();
        });

    connect(new Disman::GetConfigOperation(Disman::GetConfigOperation::NoEDID),
            &Disman::ConfigOperation::finished,
            this,
            [this](Disman::ConfigOperation* op) {
                m_screenConfiguration = qobject_cast<Disman::GetConfigOperation*>(op)->config();

                Disman::ConfigMonitor::instance()->addConfig(m_screenConfiguration);
                connect(Disman::ConfigMonitor::instance(),
                        &Disman::ConfigMonitor::configurationChanged,
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
    const QMetaEnum actionEnum = QMetaEnum::fromType<Disman::OsdAction::Action>();
    Q_ASSERT(actionEnum.isValid());

    const QString presetName = QString::fromLatin1(actionEnum.valueToKey(action));
    if (presetName.isEmpty()) {
        return;
    }

    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kded5"),
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
    m_connectedOutputCount
        = std::count_if(outputs.begin(), outputs.end(), [](const Disman::OutputPtr& output) {
              return output->isConnected();
          });

    if (m_connectedOutputCount != oldConnectedOutputCount) {
        emit connectedOutputCountChanged();
    }
}

K_EXPORT_PLASMA_APPLET_WITH_JSON(kdisplay, KDisplayApplet, "metadata.json")

#include "kdisplay_applet.moc"
