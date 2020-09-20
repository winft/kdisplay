/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2016 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2018 Kai Uwe Broulik <kde@broulik.de>
    SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "daemon.h"

#include "../common/orientation_sensor.h"
#include "config.h"
#include "generator.h"
#include "kdisplay_daemon_debug.h"
#include "kdisplayadaptor.h"
#include "osdmanager.h"

#include <disman/configmonitor.h>
#include <disman/getconfigoperation.h>
#include <disman/log.h>
#include <disman/output.h>
#include <disman/setconfigoperation.h>

#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>
#include <QOrientationReading>

K_PLUGIN_CLASS_WITH_JSON(KDisplayDaemon, "kdisplayd.json")

KDisplayDaemon::KDisplayDaemon(QObject* parent, const QList<QVariant>&)
    : KDEDModule(parent)
    , m_monitoring{false}
    , m_orientationSensor(new OrientationSensor(this))
{
    Disman::Log::instance();

    connect(new Disman::GetConfigOperation,
            &Disman::GetConfigOperation::finished,
            this,
            &KDisplayDaemon::init);
}

void KDisplayDaemon::init(Disman::ConfigOperation* op)
{
    if (op->has_error()) {
        qCWarning(KDISPLAY_KDED) << "Initial config has error.";
        return;
    }

    m_monitoredConfig = qobject_cast<Disman::GetConfigOperation*>(op)->config();
    auto cfg = m_monitoredConfig.get();

    qCDebug(KDISPLAY_KDED) << "Config" << cfg << "is ready";
    Disman::ConfigMonitor::instance()->add_config(m_monitoredConfig);

    update_auto_rotate();
    setMonitorForChanges(true);

    KActionCollection* coll = new KActionCollection(this);
    QAction* action = coll->addAction(QStringLiteral("display"));
    action->setText(i18n("Switch Display"));
    QList<QKeySequence> switchDisplayShortcuts({Qt::Key_Display, Qt::MetaModifier + Qt::Key_P});
    KGlobalAccel::self()->setGlobalShortcut(action, switchDisplayShortcuts);
    connect(action, &QAction::triggered, this, &KDisplayDaemon::displayButton);

    new KdisplayAdaptor(this);
    // Initialize OSD manager to register its dbus interface
    m_osdManager = new OsdManager(this);

    connect(cfg, &Disman::Config::output_added, this, &KDisplayDaemon::applyConfig);
    connect(cfg, &Disman::Config::output_removed, this, &KDisplayDaemon::applyConfig);

    connect(m_orientationSensor,
            &OrientationSensor::availableChanged,
            this,
            &KDisplayDaemon::updateOrientation);
    connect(m_orientationSensor,
            &OrientationSensor::valueChanged,
            this,
            &KDisplayDaemon::updateOrientation);

    applyConfig();

    m_startingUp = false;
}

void KDisplayDaemon::update_auto_rotate()
{
    assert(m_monitoredConfig);
    m_orientationSensor->setEnabled(Config(m_monitoredConfig).autoRotationRequested());
}

void KDisplayDaemon::updateOrientation()
{
    assert(m_monitoredConfig);

    const auto features = m_monitoredConfig->supported_features();
    if (!features.testFlag(Disman::Config::Feature::AutoRotation)
        || !features.testFlag(Disman::Config::Feature::TabletMode)) {
        return;
    }

    if (!m_orientationSensor->available() || !m_orientationSensor->enabled()) {
        return;
    }

    const auto orientation = m_orientationSensor->value();
    if (orientation == QOrientationReading::Undefined) {
        // Orientation sensor went off. Do not change current orientation.
        return;
    }
    if (orientation == QOrientationReading::FaceUp
        || orientation == QOrientationReading::FaceDown) {
        // We currently don't do anything with FaceUp/FaceDown, but in the future we could use them
        // to shut off and switch on again a display when display is facing downwards/upwards.
        return;
    }

    Config(m_monitoredConfig).setDeviceOrientation(orientation);
    if (m_monitoring) {
        doApplyConfig(m_monitoredConfig);
    } else {
        m_configDirty = true;
    }
}

void KDisplayDaemon::doApplyConfig(Disman::ConfigPtr const& config)
{
    qCDebug(KDISPLAY_KDED) << "Do set and apply specific config";

    m_monitoredConfig->apply(config);
    refreshConfig();
}

void KDisplayDaemon::refreshConfig()
{
    setMonitorForChanges(false);
    m_configDirty = false;
    Disman::ConfigMonitor::instance()->add_config(m_monitoredConfig);

    connect(new Disman::SetConfigOperation(m_monitoredConfig),
            &Disman::SetConfigOperation::finished,
            this,
            [this]() {
                qCDebug(KDISPLAY_KDED) << "Config applied";
                if (m_configDirty) {
                    // Config changed in the meantime again, apply.
                    doApplyConfig(m_monitoredConfig);
                } else {
                    setMonitorForChanges(true);
                }
            });
}

void KDisplayDaemon::applyConfig()
{
    qCDebug(KDISPLAY_KDED) << "Applying config";

    const bool showOsd = m_monitoredConfig->outputs().size() > 1 && !m_startingUp
        && m_monitoredConfig->cause() == Disman::Config::Cause::generated;

    if (showOsd) {
        qCDebug(KDISPLAY_KDED) << "Getting ideal config from user via OSD...";
        auto action = m_osdManager->showActionSelector();
        connect(action, &OsdAction::selected, this, &KDisplayDaemon::applyOsdAction);
    } else {
        m_osdManager->hideOsd();
    }
}

void KDisplayDaemon::applyLayoutPreset(const QString& presetName)
{
    const QMetaEnum actionEnum = QMetaEnum::fromType<OsdAction::Action>();
    Q_ASSERT(actionEnum.isValid());

    bool ok;
    auto action
        = static_cast<OsdAction::Action>(actionEnum.keyToValue(qPrintable(presetName), &ok));
    if (!ok) {
        qCWarning(KDISPLAY_KDED) << "Cannot apply unknown screen layout preset named" << presetName;
        return;
    }
    applyOsdAction(action);
}

bool KDisplayDaemon::getAutoRotate()
{
    return Config(m_monitoredConfig).getAutoRotate();
}

void KDisplayDaemon::setAutoRotate(bool value)
{
    if (!m_monitoredConfig) {
        return;
    }
    Config(m_monitoredConfig).setAutoRotate(value);
    m_orientationSensor->setEnabled(value);
}

void KDisplayDaemon::applyOsdAction(OsdAction::Action action)
{
    qCDebug(KDISPLAY_KDED) << "Applying OSD action:" << action;

    if (auto config = Generator::displaySwitch(action, m_monitoredConfig)) {
        doApplyConfig(config);
    }
}

void KDisplayDaemon::configChanged()
{
    qCDebug(KDISPLAY_KDED) << "Change detected" << m_monitoredConfig;

    update_auto_rotate();
    updateOrientation();
}

void KDisplayDaemon::showOsd(const QString& icon, const QString& text)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QLatin1String("org.kde.plasmashell"),
                                                      QLatin1String("/org/kde/osdService"),
                                                      QLatin1String("org.kde.osdService"),
                                                      QLatin1String("showText"));
    msg << icon << text;
    QDBusConnection::sessionBus().asyncCall(msg);
}

void KDisplayDaemon::showOutputIdentifier()
{
    m_osdManager->showOutputIdentifiers();
}

void KDisplayDaemon::displayButton()
{
    qCDebug(KDISPLAY_KDED) << "displayBtn triggered";

    auto action = m_osdManager->showActionSelector();
    connect(action, &OsdAction::selected, this, &KDisplayDaemon::applyOsdAction);
}

void KDisplayDaemon::setMonitorForChanges(bool enabled)
{
    if (m_monitoring == enabled) {
        return;
    }

    qCDebug(KDISPLAY_KDED) << "Monitor for changes: " << enabled;
    m_monitoring = enabled;

    if (m_monitoring) {
        connect(Disman::ConfigMonitor::instance(),
                &Disman::ConfigMonitor::configuration_changed,
                this,
                &KDisplayDaemon::configChanged,
                Qt::UniqueConnection);
    } else {
        disconnect(Disman::ConfigMonitor::instance(),
                   &Disman::ConfigMonitor::configuration_changed,
                   this,
                   &KDisplayDaemon::configChanged);
    }
}

#include "daemon.moc"
