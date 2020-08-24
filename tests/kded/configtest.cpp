/********************************************************************
Copyright 2015 Daniel Vrátil <dvratil@redhat.com>
Copyright 2018 Roman Gilg <subdiff@gmail.com>

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
#include "../../kded/config.h"
#include "../../common/globals.h"

#include <QObject>
#include <QtTest>

#include <disman/config.h>
#include <disman/edid.h>
#include <disman/mode.h>
#include <disman/output.h>
#include <disman/screen.h>

#include <memory>

struct TestPathGuard {
    TestPathGuard()
    {
        // TODO: this should setup of the control directory.
        QStandardPaths::setTestModeEnabled(true);
        path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
            % QStringLiteral("/kdisplay/");
        Globals::setDirPath(path);
    }
    ~TestPathGuard()
    {
        QVERIFY(!path.isEmpty());
        QVERIFY(QDir(path).removeRecursively());
    }

    QString path;
};

class TestConfig : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void initTestCase();

    void testSimpleConfig();
    void testTwoScreenConfig();
    void testRotatedScreenConfig();
    void testDisabledScreenConfig();
    void testConfig404();
    void testCorruptConfig();
    void testCorruptEmptyConfig();
    void testCorruptUselessConfig();
    void testNullConfig();
    void testIdenticalOutputs();
    void testMoveConfig();
    void testFixedConfig();

private:
    std::unique_ptr<Config> createConfig(bool output1Enabled, bool output2Enabled);
};

std::unique_ptr<Config> TestConfig::createConfig(bool output1Enabled, bool output2Enabled)
{
    Disman::ScreenPtr screen = Disman::ScreenPtr::create();
    screen->setCurrentSize(QSize(1920, 1080));
    screen->setMaxSize(QSize(32768, 32768));
    screen->setMinSize(QSize(8, 8));

    QList<QSize> sizes(
        {QSize(320, 240), QSize(640, 480), QSize(1024, 768), QSize(1280, 1024), QSize(1920, 1280)});
    Disman::ModeList modes;
    for (int i = 0; i < sizes.count(); ++i) {
        const QSize& size = sizes[i];
        Disman::ModePtr mode = Disman::ModePtr::create();
        mode->setId(QStringLiteral("MODE-%1").arg(i));
        mode->setName(QStringLiteral("%1x%2").arg(size.width()).arg(size.height()));
        mode->setSize(size);
        mode->setRefreshRate(60.0);
        modes.insert(mode->id(), mode);
    }

    Disman::OutputPtr output1 = Disman::OutputPtr::create();
    output1->setId(1);
    output1->setName(QStringLiteral("OUTPUT-1"));
    output1->setPosition(QPoint(0, 0));
    output1->setEnabled(output1Enabled);
    output1->setModes(modes);

    Disman::OutputPtr output2 = Disman::OutputPtr::create();
    output2->setId(2);
    output2->setName(QStringLiteral("OUTPUT-2"));
    output2->setPosition(QPoint(0, 0));
    output2->setEnabled(output2Enabled);
    output2->setModes(modes);

    Disman::ConfigPtr config = Disman::ConfigPtr::create();
    config->setScreen(screen);
    config->addOutput(output1);
    config->addOutput(output2);

    auto configWrapper = std::unique_ptr<Config>(new Config(config));
    return configWrapper;
}

void TestConfig::init()
{
    Globals::setDirPath(QStringLiteral(TEST_DATA "serializerdata/"));
}

void TestConfig::initTestCase()
{
    qputenv("DISMAN_LOGGING", "false");
}

void TestConfig::testSimpleConfig()
{
    auto configWrapper = createConfig(true, false);
    configWrapper = configWrapper->readFile(QStringLiteral("simpleConfig.json"));

    auto config = configWrapper->data();
    QVERIFY(config);
    QCOMPARE(config->outputs().count(), 2);

    auto output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(output->isPrimary(), true);

    auto output2 = config->outputs().last();
    QCOMPARE(output2->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output2->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output2->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output2->isEnabled(), false);
    QCOMPARE(output2->rotation(), Disman::Output::None);
    QCOMPARE(output2->position(), QPoint(0, 0));
    QCOMPARE(output2->isPrimary(), false);

    auto screen = config->screen();
    QCOMPARE(screen->currentSize(), QSize(1920, 1280));
}

void TestConfig::testTwoScreenConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("twoScreenConfig.json"));

    auto config = configWrapper->data();
    QVERIFY(config);

    QCOMPARE(config->outputs().count(), 2);

    auto output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(output->isPrimary(), true);

    output = config->outputs().last();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-3"));
    QCOMPARE(output->auto_mode()->size(), QSize(1280, 1024));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(1920, 0));
    QCOMPARE(output->isPrimary(), false);

    auto screen = config->screen();
    QCOMPARE(screen->currentSize(), QSize(3200, 1280));
}

void TestConfig::testRotatedScreenConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("rotatedScreenConfig.json"));

    auto config = configWrapper->data();
    QVERIFY(config);

    QCOMPARE(config->outputs().count(), 2);

    auto output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(output->isPrimary(), true);

    output = config->outputs().last();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-3"));
    QCOMPARE(output->auto_mode()->size(), QSize(1280, 1024));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::Left);
    QCOMPARE(output->position(), QPoint(1920, 0));
    QCOMPARE(output->isPrimary(), false);

    auto screen = config->screen();
    QCOMPARE(screen->currentSize(), QSize(2944, 1280));
}

void TestConfig::testDisabledScreenConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("disabledScreenConfig.json"));

    auto config = configWrapper->data();
    QVERIFY(config);

    QCOMPARE(config->outputs().count(), 2);

    auto output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(output->isPrimary(), true);

    output = config->outputs().last();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output->isEnabled(), false);

    auto screen = config->screen();
    QCOMPARE(screen->currentSize(), QSize(1920, 1280));
}

void TestConfig::testConfig404()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("filenotfoundConfig.json"));

    QVERIFY(!configWrapper);
}

void TestConfig::testCorruptConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("corruptConfig.json"));
    auto config = configWrapper->data();

    QVERIFY(config);
    QCOMPARE(config->outputs().count(), 2);
    QVERIFY(config->isValid());
}

void TestConfig::testCorruptEmptyConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("corruptEmptyConfig.json"));
    auto config = configWrapper->data();

    QVERIFY(config);
    QCOMPARE(config->outputs().count(), 2);
    QVERIFY(config->isValid());
}

void TestConfig::testCorruptUselessConfig()
{
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("corruptUselessConfig.json"));
    auto config = configWrapper->data();

    QVERIFY(config);
    QCOMPARE(config->outputs().count(), 2);
    QVERIFY(config->isValid());
}

void TestConfig::testNullConfig()
{
    Config nullConfig(nullptr);
    QVERIFY(!nullConfig.data());

    // Null configs have empty configIds
    QVERIFY(nullConfig.id().isEmpty());

    // Load config from a file not found results in a nullptr
    auto config = createConfig(true, true);
    QVERIFY(!config->readFile(QString()));

    auto guard = TestPathGuard();

    // Wrong config file name should fail to save
    QVERIFY(!config->writeFile(QString()));
}

void TestConfig::testIdenticalOutputs()
{
    // Test configuration of a video wall with 6 identical outputs connected
    // this is the autotest for https://bugs.kde.org/show_bug.cgi?id=325277
    Disman::ScreenPtr screen = Disman::ScreenPtr::create();
    screen->setCurrentSize(QSize(1920, 1080));
    screen->setMaxSize(QSize(32768, 32768));
    screen->setMinSize(QSize(8, 8));

    QList<QSize> sizes({QSize(640, 480),
                        QSize(1024, 768),
                        QSize(1920, 1080),
                        QSize(1280, 1024),
                        QSize(1920, 1280)});
    Disman::ModeList modes;
    for (int i = 0; i < sizes.count(); ++i) {
        const QSize& size = sizes[i];
        Disman::ModePtr mode = Disman::ModePtr::create();
        mode->setId(QStringLiteral("MODE-%1").arg(i));
        mode->setName(QStringLiteral("%1x%2").arg(size.width()).arg(size.height()));
        mode->setSize(size);
        mode->setRefreshRate(60.0);
        modes.insert(mode->id(), mode);
    }
    // This one is important, the output id in the config file is a hash of it
    QByteArray data = QByteArray::fromBase64(
        "AP///////"
        "wAQrBbwTExLQQ4WAQOANCB46h7Frk80sSYOUFSlSwCBgKlA0QBxTwEBAQEBAQEBKDyAoHCwI0AwIDYABkQhAAAaAAA"
        "A/wBGNTI1TTI0NUFLTEwKAAAA/ABERUxMIFUyNDEwCiAgAAAA/"
        "QA4TB5REQAKICAgICAgAToCAynxUJAFBAMCBxYBHxITFCAVEQYjCQcHZwMMABAAOC2DAQAA4wUDAQI6gBhxOC1AWCx"
        "FAAZEIQAAHgEdgBhxHBYgWCwlAAZEIQAAngEdAHJR0B4gbihVAAZEIQAAHowK0Iog4C0QED6WAAZEIQAAGAAAAAAAA"
        "AAAAAAAAAAAPg==");

    // When setting up the outputs, make sure they're not added in alphabetical order
    // or in the same order of the config file, as that makes the tests accidentally pass

    Disman::OutputPtr output1 = Disman::OutputPtr::create();
    output1->setId(1);
    output1->setEdid(data);
    output1->setName(QStringLiteral("DisplayPort-0"));
    output1->setPosition(QPoint(0, 0));
    output1->setEnabled(false);
    output1->setModes(modes);

    Disman::OutputPtr output2 = Disman::OutputPtr::create();
    output2->setId(2);
    output2->setEdid(data);
    output2->setName(QStringLiteral("DisplayPort-1"));
    output2->setPosition(QPoint(0, 0));
    output2->setEnabled(false);
    output2->setModes(modes);

    Disman::OutputPtr output3 = Disman::OutputPtr::create();
    output3->setId(3);
    output3->setEdid(data);
    output3->setName(QStringLiteral("DisplayPort-2"));
    output3->setPosition(QPoint(0, 0));
    output3->setEnabled(false);
    output3->setModes(modes);

    Disman::OutputPtr output6 = Disman::OutputPtr::create();
    output6->setId(6);
    output6->setEdid(data);
    output6->setName(QStringLiteral("DVI-0"));
    output6->setPosition(QPoint(0, 0));
    output6->setEnabled(false);
    output6->setModes(modes);

    Disman::OutputPtr output4 = Disman::OutputPtr::create();
    output4->setId(4);
    output4->setEdid(data);
    output4->setName(QStringLiteral("DisplayPort-3"));
    output4->setPosition(QPoint(0, 0));
    output4->setEnabled(false);
    output4->setModes(modes);

    Disman::OutputPtr output5 = Disman::OutputPtr::create();
    output5->setId(5);
    output5->setEdid(data);
    output5->setName(QStringLiteral("DVI-1"));
    output5->setPosition(QPoint(0, 0));
    output5->setEnabled(false);
    output5->setModes(modes);

    Disman::ConfigPtr config = Disman::ConfigPtr::create();
    config->setScreen(screen);
    config->addOutput(output6);
    config->addOutput(output2);
    config->addOutput(output5);
    config->addOutput(output4);
    config->addOutput(output3);
    config->addOutput(output1);

    Config configWrapper(config);

    QHash<QString, QPoint> positions;
    positions[QStringLiteral("DisplayPort-0")] = QPoint(0, 1080);
    positions[QStringLiteral("DisplayPort-1")] = QPoint(2100, 30);
    positions[QStringLiteral("DisplayPort-2")] = QPoint(2100, 1080);
    positions[QStringLiteral("DisplayPort-3")] = QPoint(4020, 0);
    positions[QStringLiteral("DVI-0")] = QPoint(4020, 1080);
    positions[QStringLiteral("DVI-1")] = QPoint(0, 0);

    auto configWrapper2 = configWrapper.readFile(QStringLiteral("outputgrid_2x3.json"));
    Disman::ConfigPtr config2 = configWrapper2->data();
    QVERIFY(config2);
    QVERIFY(config != config2);

    QCOMPARE(config2->outputs().count(), 6);
    Q_FOREACH (auto output, config2->outputs()) {
        QVERIFY(positions.keys().contains(output->name()));
        QVERIFY(output->name() != output->hash());
        QCOMPARE(positions[output->name()], output->position());
        QCOMPARE(output->auto_mode()->size(), QSize(1920, 1080));
        QCOMPARE(output->auto_mode()->refreshRate(), 60.0);
        QVERIFY(output->isEnabled());
    }
    QCOMPARE(config2->screen()->currentSize(), QSize(5940, 2160));
}

void TestConfig::testMoveConfig()
{
    // Test if restoring a config using Serializer::moveConfig(src, dest) works
    // https://bugs.kde.org/show_bug.cgi?id=353029

    // Load a dualhead config
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("twoScreenConfig.json"));

    auto config = configWrapper->data();
    QVERIFY(config);

    // Make sure we don't write into TEST_DATA.
    auto guard = TestPathGuard();

    // Basic assumptions for the remainder of our tests, this is the situation where the lid is
    // opened
    QCOMPARE(config->outputs().count(), 2);

    auto output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->isPrimary(), true);

    auto output2 = config->outputs().last();
    QCOMPARE(output2->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output2->isEnabled(), true);
    QCOMPARE(output2->isPrimary(), false);

    // we fake the lid being closed, first save our current config to _lidOpened
    configWrapper->writeOpenLidFile();

    // ... then switch off the panel, set primary to the other output
    output->setEnabled(false);
    output->setPrimary(false);
    output2->setPrimary(true);

    // save config as the current one, this is the config we don't want restored, and which we'll
    // overwrite
    configWrapper->writeFile();

    QCOMPARE(output->isEnabled(), false);
    QCOMPARE(output->isPrimary(), false);
    QCOMPARE(output2->isPrimary(), true);

    // Check if both files exist
    const QString basePath = Config::configsDirPath() + configWrapper->id();
    const QString closedPath = basePath + QStringLiteral(".json");
    const QString openedPath = basePath + QStringLiteral("_lidOpened.json");

    QFile openCfg(openedPath);
    QFile closedCfg(closedPath);
    QVERIFY(openCfg.exists());
    QVERIFY(closedCfg.exists());

    // Switcheroolooloo...
    configWrapper = configWrapper->readOpenLidFile();
    QVERIFY(configWrapper);

    // Check actual files, src should be gone, dest must exist
    QVERIFY(!openCfg.exists());
    QVERIFY(closedCfg.exists());

    // Make sure the laptop panel is enabled and primary again
    config = configWrapper->data();

    output = config->outputs().first();
    QCOMPARE(output->name(), QLatin1String("OUTPUT-1"));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->isPrimary(), true);

    output2 = config->outputs().last();
    QCOMPARE(output2->name(), QLatin1String("OUTPUT-2"));
    QCOMPARE(output2->isEnabled(), true);
    QCOMPARE(output2->isPrimary(), false);

    // Make sure we don't screw up when there's no _lidOpened config
    configWrapper = configWrapper->readOpenLidFile();
    QVERIFY(!configWrapper);
}

void TestConfig::testFixedConfig()
{
    // Load a dualhead config
    auto configWrapper = createConfig(true, true);
    configWrapper = configWrapper->readFile(QStringLiteral("twoScreenConfig.json"));
    auto config = configWrapper->data();
    QVERIFY(config);

    // Make sure we don't write into TEST_DATA.
    auto guard = TestPathGuard();

    const QString fixedCfgPath = Config::configsDirPath() % Config::s_fixedConfigFileName;
    QVERIFY(QDir().mkpath(Config::configsDirPath()));

    // save config as the current one, this is the config we don't want restored, and which we'll
    // overwrite
    configWrapper->writeFile(fixedCfgPath);

    // Check if both files exist
    QFile fixedCfg(fixedCfgPath);
    QVERIFY(fixedCfg.exists());
}

QTEST_MAIN(TestConfig)

#include "configtest.moc"
