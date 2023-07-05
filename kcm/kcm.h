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
#pragma once

#include "output_model.h"

#include <KQuickManagedConfigModule>

namespace Disman
{
class ConfigOperation;
}

class ConfigHandler;
class OrientationSensor;
class OutputIdentifier;

class KCMKDisplay : public KQuickManagedConfigModule
{
    Q_OBJECT

    Q_PROPERTY(OutputModel* outputModel READ outputModel NOTIFY outputModelChanged)
    Q_PROPERTY(bool backendReady READ backendReady NOTIFY backendReadyChanged)
    Q_PROPERTY(bool screenNormalized READ screenNormalized NOTIFY screenNormalizedChanged)
    Q_PROPERTY(bool perOutputScaling READ perOutputScaling NOTIFY perOutputScalingChanged)
    Q_PROPERTY(bool adaptiveSyncSupported READ supports_adaptive_sync NOTIFY
                   supports_adaptive_sync_changed)
    Q_PROPERTY(bool primaryOutputSupported READ primaryOutputSupported NOTIFY
                   primaryOutputSupportedChanged)
    Q_PROPERTY(bool outputReplicationSupported READ outputReplicationSupported NOTIFY
                   outputReplicationSupportedChanged)
    Q_PROPERTY(qreal globalScale READ globalScale WRITE setGlobalScale NOTIFY globalScaleChanged)
    Q_PROPERTY(int outputRetention READ outputRetention WRITE setOutputRetention NOTIFY
                   outputRetentionChanged)
    Q_PROPERTY(
        bool autoRotationSupported READ autoRotationSupported NOTIFY autoRotationSupportedChanged)
    Q_PROPERTY(bool orientationSensorAvailable READ orientationSensorAvailable NOTIFY
                   orientationSensorAvailableChanged)
    Q_PROPERTY(bool tabletModeAvailable READ tabletModeAvailable NOTIFY tabletModeAvailableChanged)

public:
    enum InvalidConfigReason {
        NoEnabledOutputs,
        ConfigHasGaps,
    };
    Q_ENUM(InvalidConfigReason)

    KCMKDisplay(QObject* parent, KPluginMetaData const& data);
    ~KCMKDisplay() override = default;

    void load() override;
    void save() override;
    void defaults() override;

    OutputModel* outputModel() const;

    Q_INVOKABLE void identifyOutputs();

    bool backendReady() const;

    Q_INVOKABLE QSize normalizeScreen() const;
    bool screenNormalized() const;

    bool perOutputScaling() const;
    bool primaryOutputSupported() const;
    bool supports_adaptive_sync() const;
    bool outputReplicationSupported() const;

    qreal globalScale() const;
    void setGlobalScale(qreal scale);

    int outputRetention() const;
    void setOutputRetention(int retention);

    bool autoRotationSupported() const;
    bool orientationSensorAvailable() const;
    bool tabletModeAvailable() const;

Q_SIGNALS:
    void backendReadyChanged();
    void backendError();
    void outputModelChanged();
    void changed();
    void screenNormalizedChanged();
    void perOutputScalingChanged();
    void supports_adaptive_sync_changed();
    void primaryOutputSupportedChanged();
    void outputReplicationSupportedChanged();
    void globalScaleChanged();
    void outputRetentionChanged();
    void autoRotationSupportedChanged();
    void orientationSensorAvailableChanged();
    void tabletModeAvailableChanged();
    void invalidConfig(InvalidConfigReason);
    void errorOnSave();
    void globalScaleWritten();
    void outputConnect(bool connected);

private:
    void setBackendReady(bool error);
    void setScreenNormalized(bool normalized);

    void fetchGlobalScale();
    void writeGlobalScale();

    void configReady(Disman::ConfigOperation* op);
    void continueNeedsSaveCheck(bool needs);

    std::unique_ptr<OutputIdentifier> m_outputIdentifier;
    std::unique_ptr<ConfigHandler> m_config;
    OrientationSensor* m_orientationSensor;
    bool m_backendReady = false;
    bool m_screenNormalized = true;
    double m_globalScale = 1.;
    double m_initialGlobalScale = 1.;

    QTimer* m_loadCompressor;
};
