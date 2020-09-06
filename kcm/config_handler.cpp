/********************************************************************
Copyright © 2019 Roman Gilg <subdiff@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#include "config_handler.h"

#include "kcm_kdisplay_debug.h"
#include "output_model.h"

#include <disman/configmonitor.h>
#include <disman/getconfigoperation.h>

#include <QRect>

using namespace Disman;

ConfigHandler::ConfigHandler(QObject* parent)
    : QObject(parent)
{
}

void ConfigHandler::setConfig(Disman::ConfigPtr config)
{
    m_config = config;
    m_initialConfig = m_config->clone();
    Disman::ConfigMonitor::instance()->add_config(m_config);

    m_outputs = new OutputModel(this);
    connect(
        m_outputs, &OutputModel::positionChanged, this, &ConfigHandler::checkScreenNormalization);
    connect(m_outputs, &OutputModel::sizeChanged, this, &ConfigHandler::checkScreenNormalization);

    for (auto const& [key, output] : config->outputs()) {
        initOutput(output);
    }
    m_lastNormalizedScreenSize = screenSize();

    connect(m_outputs, &OutputModel::changed, this, [this]() {
        checkNeedsSave();
        Q_EMIT changed();
    });
    connect(m_config.data(), &Disman::Config::output_added, this, [this]() {
        Q_EMIT outputConnect(true);
    });
    connect(m_config.data(), &Disman::Config::output_removed, this, [this]() {
        Q_EMIT outputConnect(false);
    });
    connect(m_config.data(),
            &Disman::Config::primary_output_changed,
            this,
            &ConfigHandler::primaryOutputChanged);

    Q_EMIT outputModelChanged();
}

void ConfigHandler::initOutput(const Disman::OutputPtr& output)
{
    m_outputs->add(output);
}

void ConfigHandler::updateInitialData()
{
    connect(
        new GetConfigOperation(), &GetConfigOperation::finished, this, [this](ConfigOperation* op) {
            if (op->has_error()) {
                return;
            }
            m_initialConfig = qobject_cast<GetConfigOperation*>(op)->config();
            checkNeedsSave();
        });
}

void ConfigHandler::checkNeedsSave()
{
    if (m_config->supported_features() & Disman::Config::Feature::PrimaryDisplay) {
        if (m_config->primary_output() && m_initialConfig->primary_output()) {
            if (m_config->primary_output()->hash() != m_initialConfig->primary_output()->hash()) {
                Q_EMIT needsSaveChecked(true);
                return;
            }
        } else if ((bool)m_config->primary_output() != (bool)m_initialConfig->primary_output()) {
            Q_EMIT needsSaveChecked(true);
            return;
        }
    }

    for (auto const& [key, output] : m_config->outputs()) {
        auto const hash = output->hash();
        for (auto const& [initial_key, initialOutput] : m_initialConfig->outputs()) {
            if (hash != initialOutput->hash()) {
                continue;
            }
            bool needsSave = false;
            if (output->enabled() != initialOutput->enabled()) {
                needsSave = true;
            }
            if (output->enabled()) {
                needsSave |= output->auto_mode()->id() != initialOutput->auto_mode()->id()
                    || output->position() != initialOutput->position()
                    || output->scale() != initialOutput->scale()
                    || output->rotation() != initialOutput->rotation()
                    || output->replication_source() != initialOutput->replication_source()
                    || output->retention() != initialOutput->retention()
                    || output->auto_resolution() != initialOutput->auto_resolution()
                    || output->auto_refresh_rate() != initialOutput->auto_refresh_rate()
                    || output->auto_rotate() != initialOutput->auto_rotate()
                    || output->auto_rotate_only_in_tablet_mode()
                        != initialOutput->auto_rotate_only_in_tablet_mode();
            }
            if (needsSave) {
                Q_EMIT needsSaveChecked(true);
                return;
            }
            break;
        }
    }
    Q_EMIT needsSaveChecked(false);
}

QSize ConfigHandler::screenSize() const
{
    int width = 0, height = 0;
    QSize size;

    for (auto const& [key, output] : m_config->outputs()) {
        if (!output->positionable()) {
            continue;
        }
        const int outputRight = output->geometry().right();
        const int outputBottom = output->geometry().bottom();

        if (outputRight > width) {
            width = outputRight;
        }
        if (outputBottom > height) {
            height = outputBottom;
        }
    }
    if (width > 0 && height > 0) {
        size = QSize(width, height);
    } else {
        size = QSize();
    }
    return size;
}

QSize ConfigHandler::normalizeScreen()
{
    if (!m_config) {
        return QSize();
    }
    bool changed = m_outputs->normalizePositions();

    const auto currentScreenSize = screenSize();
    changed |= m_lastNormalizedScreenSize != currentScreenSize;
    m_lastNormalizedScreenSize = currentScreenSize;

    Q_EMIT screenNormalizationUpdate(true);
    return currentScreenSize;
}

void ConfigHandler::checkScreenNormalization()
{
    const bool normalized = !m_config
        || (m_lastNormalizedScreenSize == screenSize() && m_outputs->positionsNormalized());

    Q_EMIT screenNormalizationUpdate(normalized);
}

void ConfigHandler::primaryOutputSelected(int index)
{
    Q_UNUSED(index)
    // TODO
}

void ConfigHandler::primaryOutputChanged(const Disman::OutputPtr& output)
{
    Q_UNUSED(output)
}

Disman::Output::Retention ConfigHandler::getRetention() const
{
    using Retention = Disman::Output::Retention;

    auto ret = Retention::Undefined;

    if (!m_config) {
        return ret;
    }

    const auto outputs = m_config->outputs();
    if (outputs.empty()) {
        return ret;
    }
    ret = outputs.begin()->second->retention();

    for (auto const& [key, output] : outputs) {
        const auto outputRet = output->retention();
        if (ret != outputRet) {
            // Control file with different retention values per output.
            return Retention::Undefined;
        }
    }

    if (ret == Retention::Undefined) {
        // If all outputs have undefined retention,
        // this should be displayed as global retention.
        return Retention::Global;
    }
    return ret;
}

int ConfigHandler::retention() const
{
    return static_cast<int>(getRetention());
}

void ConfigHandler::setRetention(int retention)
{
    using Retention = Disman::Output::Retention;

    if (!m_config) {
        return;
    }

    if (retention != static_cast<int>(Retention::Global)
        && retention != static_cast<int>(Retention::Individual)) {
        // We only allow setting to global or individual retention.
        return;
    }

    if (retention == ConfigHandler::retention()) {
        return;
    }

    auto ret = static_cast<Retention>(retention);
    for (auto const& [key, output] : m_config->outputs()) {
        output->set_retention(ret);
    }
    checkNeedsSave();
    Q_EMIT retentionChanged();
    Q_EMIT changed();
}
