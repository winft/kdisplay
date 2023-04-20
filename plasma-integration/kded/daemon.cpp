/*
    SPDX-FileCopyrightText: 2012 Alejandro Fiestas Olivares <afiestas@kde.org>
    SPDX-FileCopyrightText: 2016 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2018 Kai Uwe Broulik <kde@broulik.de>
    SPDX-FileCopyrightText: 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "daemon.h"

#include "../../common/orientation_sensor.h"
#include "config.h"
#include "generator.h"
#include "kdisplay_daemon_debug.h"
#include "kdisplayadaptor.h"
#include "osdservice_interface.h"

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
    qMetaTypeId<KDisplay::OsdAction>();

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
    QList<QKeySequence> switchDisplayShortcuts({Qt::Key_Display, Qt::MetaModifier | Qt::Key_P});
    KGlobalAccel::self()->setGlobalShortcut(action, switchDisplayShortcuts);
    connect(action, &QAction::triggered, this, &KDisplayDaemon::displayButton);

    new KdisplayAdaptor(this);

    QString const osdService = QStringLiteral("org.kwinft.kdisplay.osdService");
    QString const osdPath = QStringLiteral("/org/kwinft/kdisplay/osdService");
    m_osdServiceInterface = new OrgKwinftKdisplayOsdServiceInterface(
        osdService, osdPath, QDBusConnection::sessionBus(), this);

    // Set a longer timeout to not assume timeout while the osd is still shown
    m_osdServiceInterface->setTimeout(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(60)).count());

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

    auto const should_show_osd = m_monitoredConfig->outputs().size() > 1 && !m_startingUp
        && m_monitoredConfig->cause() == Disman::Config::Cause::generated;

    if (should_show_osd) {
        qCDebug(KDISPLAY_KDED) << "Getting ideal config from user via OSD...";
        show_osd();
    } else {
        m_osdServiceInterface->hideOsd();
    }
}

void KDisplayDaemon::applyLayoutPreset(const QString& presetName)
{
    auto const actionEnum = QMetaEnum::fromType<KDisplay::OsdAction::Action>();
    Q_ASSERT(actionEnum.isValid());

    bool ok;
    auto action = static_cast<KDisplay::OsdAction::Action>(
        actionEnum.keyToValue(qPrintable(presetName), &ok));
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
    if (!m_monitoredConfig || !m_orientationSensor->available()) {
        return;
    }
    Config(m_monitoredConfig).setAutoRotate(value);
    refreshConfig();
}

void KDisplayDaemon::applyOsdAction(KDisplay::OsdAction::Action action)
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

void KDisplayDaemon::show_osd()
{
    auto call = m_osdServiceInterface->showActionSelector();
    auto watcher = new QDBusPendingCallWatcher(call);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();
        QDBusReply<int> reply = *watcher;
        if (!reply.isValid()) {
            return;
        }
        applyOsdAction(static_cast<KDisplay::OsdAction::Action>(reply.value()));
    });
}

void KDisplayDaemon::displayButton()
{
    qCDebug(KDISPLAY_KDED) << "displayBtn triggered";
    show_osd();
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
