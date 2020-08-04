/********************************************************************
Copyright 2019 Roman Gilg <subdiff@gmail.com>

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
#ifndef KDED_CONFIG_H
#define KDED_CONFIG_H

#include <disman/config.h>

#include <QOrientationReading>

#include <memory>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(Disman::ConfigPtr config, QObject* parent = nullptr);
    ~Config() = default;

    QString id() const;

    bool fileExists() const;
    std::unique_ptr<Config> readFile();
    std::unique_ptr<Config> readOpenLidFile();
    bool writeFile();
    bool writeOpenLidFile();

    Disman::ConfigPtr data() const
    {
        return m_data;
    }

    bool autoRotationRequested() const;
    void setDeviceOrientation(QOrientationReading::Orientation orientation);
    bool getAutoRotate() const;
    void setAutoRotate(bool value);
    void log();

    void setValidityFlags(Disman::Config::ValidityFlags flags)
    {
        m_validityFlags = flags;
    }

    bool canBeApplied() const;

private:
    friend class TestConfig;

    QString createPath(const QString& fileName);
    QString path(const QString& fileName) const;
    QString fileName() const;
    QString openLidFileName() const;

    std::unique_ptr<Config> readFile(const QString& fileName);
    bool writeFile(const QString& filePath);

    bool canBeApplied(Disman::ConfigPtr config) const;

    Disman::ConfigPtr m_data;
    Disman::Config::ValidityFlags m_validityFlags;

    static QString s_configsDirName;
    static QString s_fixedConfigFileName;

    static QString configsDirPath();
};

#endif
