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

    void laptopLidOpenAndExternal();
    void laptopLidOpenAndTwoExternal();
    void laptopLidClosedAndExternal();
    void laptopLidClosedAndThreeExternal();
    void laptopDockedLidOpenAndExternal();
    void laptopDockedLidClosedAndExternal();

    void switchDisplayTwoScreens();
};

Disman::ConfigPtr testScreenConfig::loadConfig(const QByteArray& fileName)
{
    Disman::BackendManager::instance()->shutdownBackend();

    QByteArray path(TEST_DATA "configs/" + fileName);
    qputenv("DISMAN_BACKEND_ARGS", "TEST_DATA=" + path);

    Disman::GetConfigOperation* op = new Disman::GetConfigOperation;
    if (!op->exec()) {
        qWarning() << op->errorString();
        return ConfigPtr();
    }
    return op->config();
}

void testScreenConfig::initTestCase()
{
    qputenv("DISMAN_LOGGING", "false");
    setenv("DISMAN_BACKEND", "Fake", 1);
}

void testScreenConfig::cleanupTestCase()
{
    Disman::BackendManager::instance()->shutdownBackend();
}

void testScreenConfig::laptopLidOpenAndExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopAndExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);

    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));

    QCOMPARE(external->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(external->isPrimary(), false);
    QCOMPARE(external->isEnabled(), false);
    QCOMPARE(external->position(), QPoint(1280, 0));
}

void testScreenConfig::laptopLidOpenAndTwoExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopLidOpenAndTwoExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr hdmi1 = config->outputs().value(2);
    OutputPtr hdmi2 = config->outputs().value(3);

    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));

    QCOMPARE(hdmi1->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(hdmi1->isPrimary(), false);
    QCOMPARE(hdmi1->isEnabled(), false);
    QCOMPARE(hdmi1->position(), QPoint(1280, 0));

    QCOMPARE(hdmi2->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(hdmi2->isPrimary(), false);
    QCOMPARE(hdmi2->isEnabled(), false);
    QCOMPARE(hdmi2->position(),
             QPoint(hdmi1->position().x() + hdmi1->auto_mode()->size().width(), 0));
}

void testScreenConfig::laptopLidClosedAndExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopAndExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);
    generator->setForceLidClosed(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);

    QCOMPARE(laptop->isEnabled(), false);
    QCOMPARE(laptop->isPrimary(), false);

    QCOMPARE(external->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(external->isPrimary(), true);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));
}

void testScreenConfig::laptopLidClosedAndThreeExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopLidClosedAndThreeExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);
    generator->setForceLidClosed(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr hdmi1 = config->outputs().value(2);
    OutputPtr hdmi2 = config->outputs().value(3);
    OutputPtr primary = config->outputs().value(4);

    QCOMPARE(laptop->isEnabled(), false);
    QCOMPARE(laptop->isPrimary(), false);

    QCOMPARE(hdmi1->isEnabled(), false);
    QCOMPARE(hdmi1->isPrimary(), false);
    QCOMPARE(hdmi1->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(hdmi1->position(), QPoint(laptop->auto_mode()->size().width(), 0));

    QCOMPARE(hdmi2->isEnabled(), false);
    QCOMPARE(hdmi2->isPrimary(), false);
    QCOMPARE(hdmi2->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(hdmi2->position(),
             QPoint(hdmi1->position().x() + hdmi1->auto_mode()->size().width(), 0));

    QCOMPARE(primary->isEnabled(), true);
    QCOMPARE(primary->isPrimary(), true);
    QCOMPARE(primary->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(primary->position(), QPoint(0, 0));
}

void testScreenConfig::laptopDockedLidOpenAndExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopAndExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);
    generator->setForceLidClosed(false);
    generator->setForceDocked(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);

    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), false);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));

    QCOMPARE(external->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(external->isPrimary(), true);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(1280, 0));
}

void testScreenConfig::laptopDockedLidClosedAndExternal()
{
    const ConfigPtr currentConfig = loadConfig("laptopAndExternal.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);
    generator->setForceLidClosed(true);
    generator->setForceDocked(true);

    ConfigPtr config = generator->idealConfig(currentConfig);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);

    QCOMPARE(laptop->isEnabled(), false);
    QCOMPARE(laptop->isPrimary(), false);

    QCOMPARE(external->auto_mode()->id(), QLatin1String("4"));
    QCOMPARE(external->isPrimary(), true);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));
}

void testScreenConfig::switchDisplayTwoScreens()
{
    const ConfigPtr currentConfig = loadConfig("switchDisplayTwoScreens.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    generator->setForceLaptop(true);
    generator->setForceNotLaptop(false);
    generator->setForceDocked(false);
    generator->setForceLidClosed(false);

    // Clone all
    ConfigPtr config = generator->displaySwitch(Generator::Clone);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);
    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(laptop->replicationSource(), 0);

    QCOMPARE(external->auto_mode()->id(), QLatin1String("5"));
    QCOMPARE(external->isPrimary(), false);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(1280, 0));
    QCOMPARE(external->replicationSource(), 1);

    // Extend to left
    config = generator->displaySwitch(Generator::ExtendToLeft);
    laptop = config->outputs().value(1);
    external = config->outputs().value(2);
    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), QLatin1String("5"));
    QCOMPARE(external->isPrimary(), false);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(-1920, 0));

    // Disable embedded,. enable external
    config = generator->displaySwitch(Generator::TurnOffEmbedded);
    laptop = config->outputs().value(1);
    external = config->outputs().value(2);
    ;
    QCOMPARE(laptop->isEnabled(), false);
    QCOMPARE(external->auto_mode()->id(), QLatin1String("5"));
    QCOMPARE(external->isPrimary(), true);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));

    // Enable embedded, disable external
    config = generator->displaySwitch(Generator::TurnOffExternal);
    QVERIFY(!config);

    // Extend to right
    config = generator->displaySwitch(Generator::ExtendToRight);
    laptop = config->outputs().value(1);
    external = config->outputs().value(2);
    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), QLatin1String("5"));
    QCOMPARE(external->isPrimary(), false);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(1280, 0));
}

void testScreenConfig::switchDisplayTwoScreensNoCommonMode()
{
    const ConfigPtr currentConfig = loadConfig("switchDisplayTwoScreensNoCommonMode.json");
    QVERIFY(currentConfig);

    Generator* generator = Generator::self();
    generator->setCurrentConfig(currentConfig);
    ConfigPtr config = generator->displaySwitch(Generator::Clone);
    OutputPtr laptop = config->outputs().value(1);
    OutputPtr external = config->outputs().value(2);

    QCOMPARE(laptop->auto_mode()->id(), QLatin1String("3"));
    QCOMPARE(laptop->isPrimary(), true);
    QCOMPARE(laptop->isEnabled(), true);
    QCOMPARE(laptop->position(), QPoint(0, 0));
    QCOMPARE(external->auto_mode()->id(), QLatin1String("5"));
    QCOMPARE(external->isPrimary(), false);
    QCOMPARE(external->isEnabled(), true);
    QCOMPARE(external->position(), QPoint(0, 0));
}

QTEST_MAIN(testScreenConfig)

#include "testgenerator.moc"
