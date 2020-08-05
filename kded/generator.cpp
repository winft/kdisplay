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
#include "generator.h"
#include "device.h"
#include "kdisplay_daemon_debug.h"
#include <QRect>

#include <disman/config.h>

#include <algorithm>

#if defined(QT_NO_DEBUG)
#define ASSERT_OUTPUTS(outputs)
#else
#define ASSERT_OUTPUTS(outputs)                                                                    \
    while (true) {                                                                                 \
        assert(!outputs.isEmpty());                                                                \
        for (Disman::OutputPtr const& output : outputs) {                                          \
            assert(output);                                                                        \
            assert(output->id());                                                                  \
            assert(output->auto_mode());                                                           \
        }                                                                                          \
        break;                                                                                     \
    }
#endif

Generator* Generator::instance = nullptr;

bool operator<(const QSize& s1, const QSize& s2)
{
    return s1.width() * s1.height() < s2.width() * s2.height();
}

Generator* Generator::self()
{
    if (!Generator::instance) {
        Generator::instance = new Generator();
    }
    return Generator::instance;
}

Generator::Generator()
    : QObject()
    , m_forceLaptop(false)
    , m_forceLidClosed(false)
    , m_forceNotLaptop(false)
    , m_forceDocked(false)
{
    connect(Device::self(), &Device::ready, this, &Generator::ready);
}

void Generator::destroy()
{
    delete Generator::instance;
    Generator::instance = nullptr;
}

void Generator::setCurrentConfig(const Disman::ConfigPtr& currentConfig)
{
    m_currentConfig = currentConfig;
}

void Generator::prepare(Disman::ConfigPtr& config)
{
    auto outputs = config->connectedOutputs();

    for (auto& output : outputs) {
        // The scale will generally be independent no matter where the output is
        // scale will affect geometry, so do this first.
        if (config->supportedFeatures().testFlag(Disman::Config::Feature::PerOutputScaling)) {
            output->setScale(bestScaleForOutput(output));
        }
        output->set_auto_resolution(true);
        output->set_auto_refresh_rate(true);
    }
}

Disman::ConfigPtr Generator::idealConfig(const Disman::ConfigPtr& currentConfig)
{
    Q_ASSERT(currentConfig);

    auto config = currentConfig->clone();

    qCDebug(KDISPLAY_KDED) << "Generates ideal config for" << config->connectedOutputs().count()
                           << "displays.";

    if (config->connectedOutputs().isEmpty()) {
        qCDebug(KDISPLAY_KDED) << "No displays found. Nothing to generate.";
        return config;
    }

    prepare(config);

    auto outputs = config->connectedOutputs();

    if (outputs.count() == 1) {
        singleOutput(outputs);
        return config;
    }

    if (isLaptop()) {
        laptop(outputs);
        return fallbackIfNeeded(config);
    }

    qCDebug(KDISPLAY_KDED) << "Extend to Right";
    extendToRight(outputs);

    return fallbackIfNeeded(config);
}

Disman::ConfigPtr Generator::fallbackIfNeeded(const Disman::ConfigPtr& config)
{
    qCDebug(KDISPLAY_KDED) << "fallbackIfNeeded()";

    Disman::ConfigPtr newConfig;

    // If the ideal config can't be applied, try clonning
    if (!Disman::Config::canBeApplied(config)) {
        if (isLaptop()) {
            newConfig = displaySwitch(Generator::Clone); // Try to clone at our best
        } else {
            newConfig = config;
            Disman::OutputList connectedOutputs = config->connectedOutputs();
            if (connectedOutputs.isEmpty()) {
                return config;
            }
            connectedOutputs.value(connectedOutputs.keys().first())->setPrimary(true);
            cloneScreens(connectedOutputs);
        }
    } else {
        newConfig = config;
    }

    // If after trying to clone at our best, we fail... return current
    if (!Disman::Config::canBeApplied(newConfig)) {
        qCDebug(KDISPLAY_KDED) << "Config cannot be applied";
        newConfig = config;
    }

    return config;
}

Disman::ConfigPtr Generator::displaySwitch(DisplaySwitchAction action)
{
    qCDebug(KDISPLAY_KDED) << "Display Switch";

    auto config = m_currentConfig;
    Q_ASSERT(config);

    prepare(config);

    auto connectedOutputs = config->connectedOutputs();

    // There's not much else we can do with only one output
    if (connectedOutputs.count() < 2) {
        singleOutput(connectedOutputs);
        return config;
    }

    // We cannot try all possible combinations with two and more outputs
    if (connectedOutputs.count() > 2) {
        extendToRight(connectedOutputs);
        return config;
    }

    Disman::OutputPtr embedded, external;
    embedded = embeddedOutput(connectedOutputs);
    // If we don't have an embedded output (desktop with two external screens
    // for instance), then pretend one of them is embedded
    if (!embedded) {
        embedded = connectedOutputs.value(connectedOutputs.keys().first());
    }
    // Just to be sure
    if (embedded->modes().isEmpty()) {
        return config;
    }

    if (action == Generator::Clone) {
        qCDebug(KDISPLAY_KDED) << "Cloning";
        embedded->setPrimary(true);
        cloneScreens(connectedOutputs);
        return config;
    }

    connectedOutputs.remove(embedded->id());
    external = connectedOutputs.value(connectedOutputs.keys().first());

    // Just to be sure
    if (external->modes().isEmpty()) {
        return config;
    }

    switch (action) {
    case Generator::ExtendToLeft: {
        qCDebug(KDISPLAY_KDED) << "Extend to left";
        external->setPosition(QPointF(0, 0));
        external->setEnabled(true);

        // We must have a mode now.
        Q_ASSERT(external->auto_mode());

        auto const size = external->geometry().size();
        embedded->setPosition(QPointF(size.width(), 0));
        embedded->setEnabled(true);
        embedded->setPrimary(true);

        return config;
    }
    case Generator::TurnOffEmbedded: {
        qCDebug(KDISPLAY_KDED) << "Turn off embedded (laptop)";
        embedded->setEnabled(false);
        embedded->setPrimary(false);

        external->setEnabled(true);
        external->setPrimary(true);

        auto extMode = external->best_mode();
        Q_ASSERT(extMode);
        external->set_mode(extMode);
        return config;
    }
    case Generator::TurnOffExternal: {
        qCDebug(KDISPLAY_KDED) << "Turn off external screen";

        embedded->setPosition(QPointF(0, 0));
        embedded->setEnabled(true);
        embedded->setPrimary(true);

        external->setEnabled(false);
        external->setPrimary(false);

        return config;
    }
    case Generator::ExtendToRight: {
        qCDebug(KDISPLAY_KDED) << "Extend to the right";

        embedded->setPosition(QPointF(0, 0));
        embedded->setEnabled(true);
        embedded->setPrimary(true);

        // We must have a mode now.
        Q_ASSERT(embedded->auto_mode());

        auto const size = embedded->geometry().size();
        external->setPosition(QPointF(size.width(), 0));
        external->setEnabled(true);
        external->setPrimary(false);

        return config;
    }
    case Generator::None:  // just return config
    case Generator::Clone: // handled above
        break;
    }

    return config;
}

uint qHash(const QSize& size)
{
    return size.width() * size.height();
}

void Generator::cloneScreens(Disman::OutputList& connectedOutputs)
{
    ASSERT_OUTPUTS(connectedOutputs);
    if (connectedOutputs.isEmpty()) {
        return;
    }

    QSet<QSize> commonSizes;
    const QSize maxScreenSize = m_currentConfig->screen()->maxSize();

    for (Disman::OutputPtr const& output : connectedOutputs) {
        QSet<QSize> modeSizes;

        for (Disman::ModePtr const& mode : output->modes()) {
            const QSize size = mode->size();
            if (size.width() > maxScreenSize.width() || size.height() > maxScreenSize.height()) {
                continue;
            }
            modeSizes.insert(mode->size());
        }

        // If we have nothing to compare against
        if (commonSizes.isEmpty()) {
            commonSizes = modeSizes;
            continue;
        }

        commonSizes.intersect(modeSizes);
    }

    qCDebug(KDISPLAY_KDED) << "Common sizes: " << commonSizes;

    // Fallback to biggestMode if no common sizes have been found.
    if (commonSizes.isEmpty()) {
        for (Disman::OutputPtr& output : connectedOutputs) {
            if (output->modes().isEmpty()) {
                continue;
            }
            output->setEnabled(true);
            output->setPosition(QPointF(0, 0));
        }
        return;
    }

    // At this point, we know we have common sizes, let's get the biggest on.
    QList<QSize> commonSizeList = commonSizes.values();
    std::sort(commonSizeList.begin(), commonSizeList.end());

    const QSize biggestSize = commonSizeList.last();

    // Finally, look for the mode with biggestSize and biggest refreshRate and set it
    qCDebug(KDISPLAY_KDED) << "Biggest Size: " << biggestSize;

    for (Disman::OutputPtr output : connectedOutputs) {
        if (output->modes().isEmpty()) {
            continue;
        }
        output->set_resolution(biggestSize);
        output->set_auto_resolution(false);

        // we resolved this mode previously, so it better works
        Q_ASSERT(output->auto_mode()->size() == biggestSize);

        output->setEnabled(true);
        output->setPosition(QPointF(0, 0));
    }
}

void Generator::singleOutput(Disman::OutputList& connectedOutputs)
{
    ASSERT_OUTPUTS(connectedOutputs);
    if (connectedOutputs.isEmpty()) {
        return;
    }

    Disman::OutputPtr output = connectedOutputs.take(connectedOutputs.keys().first());
    if (output->modes().isEmpty()) {
        return;
    }

    output->setEnabled(true);
    output->setPrimary(true);
    output->setPosition(QPointF(0, 0));
}

void Generator::laptop(Disman::OutputList& connectedOutputs)
{
    ASSERT_OUTPUTS(connectedOutputs)

    qCDebug(KDISPLAY_KDED) << "Generate laptop config with" << connectedOutputs.count()
                           << "displays";

    if (connectedOutputs.isEmpty()) {
        qCDebug(KDISPLAY_KDED) << "No displays found. Nothing to generate.";
        return;
    }

    auto embedded = embeddedOutput(connectedOutputs);

    /* Apparently older laptops use "VGA-*" as embedded output ID, so embeddedOutput()
     * will fail, because it looks only for modern "LVDS", "EDP", etc. If we
     * fail to detect which output is embedded, just use the one  with the lowest
     * ID. It's a wild guess, but I think it's highly probable that it will work.
     * See bug #318907 for further reference. -- dvratil
     */
    if (!embedded) {
        QList<int> keys = connectedOutputs.keys();
        std::sort(keys.begin(), keys.end());
        embedded = connectedOutputs.value(keys.first());
    }

    connectedOutputs.remove(embedded->id());

    if (connectedOutputs.isEmpty() || embedded->modes().isEmpty()) {
        qCWarning(KDISPLAY_KDED) << "No external outputs found, going for single-output.";
        connectedOutputs.insert(embedded->id(), embedded);
        return singleOutput(connectedOutputs);
    }

    if (isLidClosed()) {
        if (connectedOutputs.count() == 1) {
            qCDebug(KDISPLAY_KDED) << "With lid closed and one other display.";
            embedded->setEnabled(false);
            embedded->setPrimary(false);

            Disman::OutputPtr external = connectedOutputs.value(connectedOutputs.keys().first());
            if (external->modes().isEmpty()) {
                return;
            }
            external->setEnabled(true);
            external->setPrimary(true);
            external->setPosition(QPointF(0, 0));
        } else {
            qCDebug(KDISPLAY_KDED) << "With lid closed and more than one other display.";
            embedded->setEnabled(false);
            embedded->setPrimary(false);

            extendToRight(connectedOutputs);
        }
        return;
    }

    qCDebug(KDISPLAY_KDED) << "Lid is open.";

    // If lid is open, laptop screen should be primary.
    embedded->setPosition(QPointF(0, 0));
    embedded->setPrimary(true);
    embedded->setEnabled(true);

    double globalWidth = embedded->geometry().width();
    Disman::OutputPtr biggest = biggestOutput(connectedOutputs);
    Q_ASSERT(biggest);
    connectedOutputs.remove(biggest->id());

    biggest->setPosition(QPointF(globalWidth, 0));
    biggest->setEnabled(true);
    biggest->setPrimary(false);

    globalWidth += biggest->geometry().width();
    for (Disman::OutputPtr& output : connectedOutputs) {
        output->setEnabled(true);
        output->setPrimary(false);
        output->setPosition(QPointF(globalWidth, 0));

        globalWidth += output->geometry().width();
    }

    if (isDocked()) {
        qCDebug(KDISPLAY_KDED) << "Docked";
        embedded->setPrimary(false);
        biggest->setPrimary(true);
    }
}

void Generator::extendToRight(Disman::OutputList& connectedOutputs)
{
    ASSERT_OUTPUTS(connectedOutputs);

    qCDebug(KDISPLAY_KDED) << "Generate config by extending to the right.";

    if (connectedOutputs.isEmpty()) {
        qCDebug(KDISPLAY_KDED) << "No displays found. Nothing to generate.";
        return;
    }

    auto biggest = biggestOutput(connectedOutputs);
    Q_ASSERT(biggest);

    connectedOutputs.remove(biggest->id());

    biggest->setEnabled(true);
    biggest->setPrimary(true);
    biggest->setPosition(QPointF(0, 0));

    double globalWidth = biggest->geometry().width();

    for (Disman::OutputPtr& output : connectedOutputs) {
        output->setEnabled(true);
        output->setPrimary(false);
        output->setPosition(QPointF(globalWidth, 0));

        globalWidth += output->geometry().width();
    }
}

double Generator::bestScaleForOutput(const Disman::OutputPtr& output)
{
    // If we have no physical size, we can't determine the DPI properly. Fallback to scale 1.
    if (output->sizeMm().height() <= 0) {
        return 1.0;
    }
    const auto mode = output->auto_mode();
    const qreal dpi = mode->size().height() / (output->sizeMm().height() / 25.4);

    // If reported DPI is closer to two times normal DPI, followed by a sanity check of having the
    // sort of vertical resolution you'd find in a high res screen.
    if (dpi > 96 * 1.5 && mode->size().height() >= 1440) {
        return 2.0;
    }
    return 1.0;
}

Disman::OutputPtr Generator::biggestOutput(const Disman::OutputList& outputs)
{
    ASSERT_OUTPUTS(outputs)

    auto max_area = 0;
    Disman::OutputPtr biggest;

    for (Disman::OutputPtr const& output : outputs) {
        auto const mode = output->best_mode();
        if (!mode) {
            continue;
        }
        auto const area = mode->size().width() * mode->size().height();
        if (area > max_area) {
            max_area = area;
            biggest = output;
        }
    }

    return biggest;
}

Disman::OutputPtr Generator::embeddedOutput(const Disman::OutputList& outputs)
{
    auto it = std::find_if(
        outputs.constBegin(), outputs.constEnd(), [](Disman::OutputPtr const& output) {
            return output->type() == Disman::Output::Panel;
        });
    return it != outputs.constEnd() ? *it : Disman::OutputPtr();
}

bool Generator::isLaptop() const
{
    if (m_forceLaptop) {
        return true;
    }
    if (m_forceNotLaptop) {
        return false;
    }

    return Device::self()->isLaptop();
}

bool Generator::isLidClosed() const
{
    if (m_forceLidClosed) {
        return true;
    }
    if (m_forceNotLaptop) {
        return false;
    }

    return Device::self()->isLidClosed();
}

bool Generator::isDocked() const
{
    if (m_forceDocked) {
        return true;
    }

    return Device::self()->isDocked();
}

void Generator::setForceLaptop(bool force)
{
    m_forceLaptop = force;
}

void Generator::setForceLidClosed(bool force)
{
    m_forceLidClosed = force;
}

void Generator::setForceDocked(bool force)
{
    m_forceDocked = force;
}

void Generator::setForceNotLaptop(bool force)
{
    m_forceNotLaptop = force;
}
