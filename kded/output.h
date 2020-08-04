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
#ifndef KDED_OUTPUT_H
#define KDED_OUTPUT_H

#include "../common/control.h"
#include "../common/globals.h"

#include <disman/types.h>

#include <QOrientationReading>
#include <QVariantMap>

class Output
{
public:
    static void readInOutputs(Disman::ConfigPtr config, const QVariantList& outputsInfo);

    static void writeGlobal(const Disman::OutputPtr& output);
    static bool writeGlobalPart(const Disman::OutputPtr& output,
                                QVariantMap& info,
                                const Disman::OutputPtr& fallback);

    static QString dirPath();

    static bool updateOrientation(Disman::OutputPtr& output,
                                  QOrientationReading::Orientation orientation);

private:
    static QString path(const QString& hash);
    static QString createPath(const QString& hash);
    static QVariantMap getGlobalData(Disman::OutputPtr output);

    static void
    readIn(Disman::OutputPtr output, const QVariantMap& info, Control::OutputRetention retention);
    static bool readInGlobal(Disman::OutputPtr output);
    static void readInGlobalPartFromInfo(Disman::OutputPtr output, const QVariantMap& info);
    /*
     * When a global output value (scale, rotation) is changed we might
     * need to reposition the outputs when another config is read.
     */
    static void adjustPositions(Disman::ConfigPtr config, const QVariantList& outputsInfo);

    static QString s_dirName;
};

#endif
