/*
    SPDX-FileCopyrightText: 2019, 2020 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KDED_CONFIG_H
#define KDED_CONFIG_H

#include <disman/types.h>

#include <QOrientationReading>

class Config
{
public:
    explicit Config(Disman::ConfigPtr config);

    bool autoRotationRequested() const;
    void setDeviceOrientation(QOrientationReading::Orientation orientation);
    bool getAutoRotate() const;
    void setAutoRotate(bool value);
    void log();

private:
    Disman::ConfigPtr m_data;
};

#endif
