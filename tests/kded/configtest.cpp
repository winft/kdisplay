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
    output1->set_name("OUTPUT-1");
    output1->set_hash("OUTPUT-1");
    output1->setPosition(QPoint(0, 0));
    output1->setEnabled(output1Enabled);
    output1->setModes(modes);

    Disman::OutputPtr output2 = Disman::OutputPtr::create();
    output2->setId(2);
    output2->set_name("OUTPUT-2");
    output2->set_hash("OUTPUT-2");
    output2->setPosition(QPoint(0, 0));
    output2->setEnabled(output2Enabled);
    output2->setModes(modes);

    Disman::ConfigPtr config = Disman::ConfigPtr::create();
    config->setScreen(screen);
    config->addOutput(output1);
    config->addOutput(output2);
    config->setPrimaryOutput(output1);

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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(config->primaryOutput(), output);

    auto output2 = config->outputs().last();
    QCOMPARE(output2->name(), "OUTPUT-2");
    QCOMPARE(output2->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output2->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output2->isEnabled(), false);
    QCOMPARE(output2->rotation(), Disman::Output::None);
    QCOMPARE(output2->position(), QPoint(0, 0));

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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(config->primaryOutput(), output);

    output = config->outputs().last();
    QCOMPARE(output->name(), "OUTPUT-2");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-3"));
    QCOMPARE(output->auto_mode()->size(), QSize(1280, 1024));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(1920, 0));

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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(config->primaryOutput(), output);

    output = config->outputs().last();
    QCOMPARE(output->name(), "OUTPUT-2");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-3"));
    QCOMPARE(output->auto_mode()->size(), QSize(1280, 1024));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::Left);
    QCOMPARE(output->position(), QPoint(1920, 0));

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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->auto_mode()->id(), QLatin1String("MODE-4"));
    QCOMPARE(output->auto_mode()->size(), QSize(1920, 1280));
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(output->rotation(), Disman::Output::None);
    QCOMPARE(output->position(), QPoint(0, 0));
    QCOMPARE(config->primaryOutput(), output);

    output = config->outputs().last();
    QCOMPARE(output->name(), "OUTPUT-2");
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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(config->primaryOutput(), output);

    auto output2 = config->outputs().last();
    QCOMPARE(output2->name(), "OUTPUT-2");
    QCOMPARE(output2->isEnabled(), true);

    // we fake the lid being closed, first save our current config to _lidOpened
    configWrapper->writeOpenLidFile();

    // ... then switch off the panel, set primary to the other output
    output->setEnabled(false);
    config->setPrimaryOutput(output2);

    // save config as the current one, this is the config we don't want restored, and which we'll
    // overwrite
    configWrapper->writeFile();

    QCOMPARE(output->isEnabled(), false);
    QCOMPARE(config->primaryOutput(), output2);

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
    QCOMPARE(output->name(), "OUTPUT-1");
    QCOMPARE(output->isEnabled(), true);
    QCOMPARE(config->primaryOutput(), output);

    output2 = config->outputs().last();
    QCOMPARE(output2->name(), "OUTPUT-2");
    QCOMPARE(output2->isEnabled(), true);

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
