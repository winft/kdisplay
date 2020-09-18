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
#include "../../kded/generator.h"

#include <QObject>
#include <QtTest>

#include <disman/backendmanager_p.h>
#include <disman/config.h>
#include <disman/getconfigoperation.h>
#include <disman/output.h>

using namespace Disman;

class testScreenConfig : public QObject
{
    Q_OBJECT

private:
    Disman::ConfigPtr loadConfig(const QByteArray& fileName);

    void switchDisplayTwoScreensNoCommonMode();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void switchDisplayTwoScreens();
};

Disman::ConfigPtr testScreenConfig::loadConfig(const QByteArray& fileName)
{
    Disman::BackendManager::instance()->shutdown_backend();

    QByteArray path(TEST_DATA "configs/" + fileName);
    qputenv("DISMAN_BACKEND_ARGS", "TEST_DATA=" + path);

    Disman::GetConfigOperation* op = new Disman::GetConfigOperation;
    if (!op->exec()) {
        qWarning() << op->error_string();
        return ConfigPtr();
    }
    return op->config();
}

void testScreenConfig::initTestCase()
{
    qputenv("DISMAN_IN_PROCESS", "1");
    qputenv("DISMAN_LOGGING", "false");
    setenv("DISMAN_BACKEND", "fake", 1);
}

void testScreenConfig::cleanupTestCase()
{
    Disman::BackendManager::instance()->shutdown_backend();
}

void testScreenConfig::switchDisplayTwoScreens()
{
    const ConfigPtr currentConfig = loadConfig("switchDisplayTwoScreens.json");
    QVERIFY(currentConfig);

    // Clone all
    auto config = Generator::displaySwitch(Generator::Action::Clone, currentConfig);
    OutputPtr laptop = config->outputs().at(1);
    OutputPtr external = config->outputs().at(2);
    QCOMPARE(laptop->auto_mode()->id(), "3");
    QCOMPARE(laptop->enabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(laptop->replication_source(), 0);
    QCOMPARE(config->primary_output(), laptop);

    QCOMPARE(external->auto_mode()->id(), "5");
    QCOMPARE(external->enabled(), true);
    QCOMPARE(external->position(), QPoint(1280, 0));
    QCOMPARE(external->replication_source(), 1);

    // Extend to left
    config = Generator::displaySwitch(Generator::Action::ExtendToLeft, currentConfig);
    laptop = config->outputs().at(1);
    external = config->outputs().at(2);
    QCOMPARE(laptop->auto_mode()->id(), "3");
    QCOMPARE(laptop->enabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), "5");
    QCOMPARE(external->enabled(), true);
    QCOMPARE(external->position(), QPoint(-1920, 0));
    QCOMPARE(config->primary_output(), laptop);

    // Disable embedded,. enable external
    config = Generator::displaySwitch(Generator::Action::TurnOffEmbedded, currentConfig);
    laptop = config->outputs().at(1);
    external = config->outputs().at(2);
    ;
    QCOMPARE(laptop->enabled(), false);
    QCOMPARE(external->auto_mode()->id(), "5");
    QCOMPARE(external->enabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));
    QCOMPARE(config->primary_output(), external);

    // Enable embedded, disable external
    config = Generator::displaySwitch(Generator::Action::TurnOffExternal, currentConfig);
    QVERIFY(!config);

    // Extend to right
    config = Generator::displaySwitch(Generator::Action::ExtendToRight, currentConfig);
    laptop = config->outputs().at(1);
    external = config->outputs().at(2);
    QCOMPARE(laptop->auto_mode()->id(), "3");
    QCOMPARE(laptop->enabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), "5");
    QCOMPARE(external->enabled(), true);
    QCOMPARE(external->position(), QPoint(1280, 0));
    QCOMPARE(config->primary_output(), laptop);
}

void testScreenConfig::switchDisplayTwoScreensNoCommonMode()
{
    const ConfigPtr currentConfig = loadConfig("switchDisplayTwoScreensNoCommonMode.json");
    QVERIFY(currentConfig);

    auto config = Generator::displaySwitch(Generator::Action::Clone, currentConfig);
    OutputPtr laptop = config->outputs().at(1);
    OutputPtr external = config->outputs().at(2);

    QCOMPARE(laptop->auto_mode()->id(), "3");
    QCOMPARE(laptop->enabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), "5");
    QCOMPARE(external->enabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));
    QCOMPARE(config->primary_output(), laptop);
}

QTEST_MAIN(testScreenConfig)

#include "testgenerator.moc"
