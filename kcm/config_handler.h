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

#include <disman/config.h>
#include <disman/output.h>

#include <memory>

class OutputModel;

class ConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit ConfigHandler(QObject* parent = nullptr);
    ~ConfigHandler() override = default;

    void setConfig(Disman::ConfigPtr config);
    void updateInitialData();

    OutputModel* outputModel() const
    {
        return m_outputs;
    }

    QSize normalizeScreen();

    Disman::ConfigPtr config() const
    {
        return m_config;
    }

    Disman::ConfigPtr initialConfig() const
    {
        return m_initialConfig;
    }

    int retention() const;
    void setRetention(int retention);

    void checkNeedsSave();

Q_SIGNALS:
    void outputModelChanged();
    void changed();
    void screenNormalizationUpdate(bool normalized);
    void needsSaveChecked(bool need);
    void retentionChanged();
    void outputConnect(bool connected);

private:
    void checkScreenNormalization();
    QSize screenSize() const;
    Disman::Output::Retention getRetention() const;
    void primaryOutputSelected(int index);
    void primaryOutputChanged(const Disman::OutputPtr& output);
    void initOutput(const Disman::OutputPtr& output);

    Disman::ConfigPtr m_config = nullptr;
    Disman::ConfigPtr m_initialConfig;
    OutputModel* m_outputs = nullptr;

    QSize m_lastNormalizedScreenSize;
};
