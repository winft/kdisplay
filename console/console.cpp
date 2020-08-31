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
#include "console.h"

#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>

#include <disman/config.h>
#include <disman/configmonitor.h>
#include <disman/edid.h>
#include <disman/getconfigoperation.h>
#include <disman/mode.h>
#include <disman/output.h>
#include <disman/types.h>

static QTextStream cout(stdout);

namespace Disman
{
namespace ConfigSerializer
{
// Exported private symbol in configserializer_p.h in Disman
extern QJsonObject serializeConfig(const Disman::ConfigPtr& config);
}
}

using namespace Disman;

Console::Console(const ConfigPtr& config)
    : QObject()
    , m_config(config)
{
}

Console::~Console()
{
}

void Console::printConfig()
{
    if (!m_config) {
        qDebug() << "Config is invalid, probably backend couldn't load";
        return;
    }
    if (!m_config->screen()) {
        qDebug() << "No screen in the configuration, broken backend";
        return;
    }

    connect(m_config.data(), &Config::primaryOutputChanged, [&](const OutputPtr& output) {
        if (output) {
            qDebug() << "New primary output: " << output->id() << output->name().c_str();
        } else {
            qDebug() << "No primary output.";
        }
    });

    qDebug() << "Screen:";
    qDebug() << "\tmaxSize:" << m_config->screen()->maxSize();
    qDebug() << "\tminSize:" << m_config->screen()->minSize();
    qDebug() << "\tcurrentSize:" << m_config->screen()->currentSize();

    OutputList outputs = m_config->outputs();
    Q_FOREACH (const OutputPtr& output, outputs) {
        qDebug() << "\n-----------------------------------------------------\n";
        qDebug() << "Id: " << output->id();
        qDebug() << "Name: " << output->name().c_str();
        qDebug() << "Type: " << typetoString(output->type());
        qDebug() << "Enabled: " << output->isEnabled();
        qDebug() << "Primary: " << output->isPrimary();
        qDebug() << "Rotation: " << output->rotation();
        qDebug() << "Pos: " << output->position();
        qDebug() << "MMSize: " << output->sizeMm();
        qDebug() << "FollowPreferredMode: " << output->followPreferredMode();
        if (output->auto_mode()) {
            qDebug() << "Size: " << output->auto_mode()->size();
        } else {
            qDebug() << "Size: No mode found!";
        }
        qDebug() << "Scale: " << output->scale();
        qDebug() << "Mode: " << output->auto_mode();
        qDebug() << "Preferred Mode: " << output->preferred_mode();
        qDebug() << "Preferred modes: " << output->preferredModes();
        qDebug() << "Modes: ";

        ModeList modes = output->modes();
        Q_FOREACH (const ModePtr& mode, modes) {
            qDebug() << "\t" << mode->id() << "  " << mode->name() << " " << mode->size() << " "
                     << mode->refreshRate();
        }

        Edid* edid = output->edid();
        qDebug() << "EDID Info: ";
        if (edid && edid->isValid()) {
            qDebug() << "\tDevice ID: " << edid->deviceId().c_str();
            qDebug() << "\tName: " << edid->name().c_str();
            qDebug() << "\tVendor: " << edid->vendor().c_str();
            qDebug() << "\tSerial: " << edid->serial().c_str();
            qDebug() << "\tEISA ID: " << edid->eisaId().c_str();
            qDebug() << "\tHash: " << edid->hash().c_str();
            qDebug() << "\tWidth: " << edid->width();
            qDebug() << "\tHeight: " << edid->height();
            qDebug() << "\tGamma: " << edid->gamma();
            qDebug() << "\tRed: " << edid->red();
            qDebug() << "\tGreen: " << edid->green();
            qDebug() << "\tBlue: " << edid->blue();
            qDebug() << "\tWhite: " << edid->white();
        } else {
            qDebug() << "\tUnavailable";
        }
    }
}

QString Console::typetoString(const Output::Type& type) const
{
    switch (type) {
    case Output::Unknown:
        return QStringLiteral("Unknown");
    case Output::Panel:
        return QStringLiteral("Panel (Laptop)");
    case Output::VGA:
        return QStringLiteral("VGA");
    case Output::DVI:
        return QStringLiteral("DVI");
    case Output::DVII:
        return QStringLiteral("DVI-I");
    case Output::DVIA:
        return QStringLiteral("DVI-A");
    case Output::DVID:
        return QStringLiteral("DVI-D");
    case Output::HDMI:
        return QStringLiteral("HDMI");
    case Output::TV:
        return QStringLiteral("TV");
    case Output::TVComposite:
        return QStringLiteral("TV-Composite");
    case Output::TVSVideo:
        return QStringLiteral("TV-SVideo");
    case Output::TVComponent:
        return QStringLiteral("TV-Component");
    case Output::TVSCART:
        return QStringLiteral("TV-SCART");
    case Output::TVC4:
        return QStringLiteral("TV-C4");
    case Output::DisplayPort:
        return QStringLiteral("DisplayPort");
    };
    return QStringLiteral("Invalid Type") + QString::number(type);
}

void Console::printJSONConfig()
{
    QJsonDocument doc(Disman::ConfigSerializer::serializeConfig(m_config));
    cout << doc.toJson(QJsonDocument::Indented);
}

void Console::printSerializations()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
        + QLatin1String("/kdisplay/");
    qDebug() << "Configs in: " << path;

    QDir dir(path);
    QStringList files = dir.entryList(QDir::Files);
    qDebug() << "Number of files: " << files.count() << Qt::endl;

    QJsonDocument parser;
    Q_FOREACH (const QString fileName, files) {
        QJsonParseError error;
        qDebug() << fileName;
        QFile file(path + QLatin1Char('/') + fileName);
        file.open(QFile::ReadOnly);
        QVariant data = parser.fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "    "
                     << "can't parse file";
            qDebug() << "    " << error.errorString();
            continue;
        }

        qDebug() << parser.toJson(QJsonDocument::Indented) << Qt::endl;
    }
}

void Console::monitor()
{
    ConfigMonitor::instance()->addConfig(m_config);
}

void Console::monitorAndPrint()
{
    monitor();
    connect(ConfigMonitor::instance(),
            &ConfigMonitor::configurationChanged,
            this,
            &Console::printConfig);
}
