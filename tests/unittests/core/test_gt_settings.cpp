/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_settings.h"
#include "gtest/gtest.h"

#include <gt_abstractsettings.h>

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

class TestSettings : public testing::Test
{
public:
    void SetUp() override
    {
        QCoreApplication::setOrganizationDomain("www.dlr.de");
        QCoreApplication::setOrganizationName("DLR");
        QCoreApplication::setApplicationName("SettingsUnitTests");
        QSettings().clear();

        settings.registerSetting("test/adouble", 123.);
    }

    void TearDown() override
    {
        QSettings().clear();
    }

    GtAbstractSettings settings;
};

TEST_F(TestSettings, setGet)
{
    settings.setSetting("test/adouble", 321.);

    auto value = settings.getSetting("test/adouble");
    ASSERT_TRUE(value.isValid());

    EXPECT_EQ(321., value.value<double>());
}

TEST_F(TestSettings, getDefault)
{
    auto value = settings.getSetting("test/adouble");
    ASSERT_TRUE(value.isValid());

    EXPECT_EQ(123., value.value<double>());
}

TEST_F(TestSettings, getUnknown)
{
    auto value = settings.getSetting("test/_unknown_");
    ASSERT_TRUE(value.isNull());
}

TEST_F(TestSettings, registerSettingRestartAndRequiresAppRestart)
{
    EXPECT_FALSE(settings.requiresAppRestart());
    ASSERT_NE(settings.registerSettingRestart("test/restart", 1), nullptr);
    EXPECT_FALSE(settings.requiresAppRestart());

    settings.setSetting("test/restart", 2);
    EXPECT_TRUE(settings.requiresAppRestart());

    settings.setSetting("test/restart", 1);
    EXPECT_TRUE(settings.requiresAppRestart());

    settings.setSetting("test/restart", 1);
    EXPECT_FALSE(settings.requiresAppRestart());
}

TEST_F(TestSettings, duplicateRegisterSettingReplacesExistingDefault)
{
    ASSERT_NE(settings.registerSetting("test/adouble", 456.), nullptr);

    auto value = settings.getSetting("test/adouble");
    ASSERT_TRUE(value.isValid());
    EXPECT_EQ(456., value.toDouble());
}

TEST_F(TestSettings, hasSettingAndGetAllSettingIds)
{
    settings.registerSetting("test/another", true);

    EXPECT_TRUE(settings.hasSetting("test/adouble"));
    EXPECT_TRUE(settings.hasSetting("test/another"));
    EXPECT_FALSE(settings.hasSetting("test/missing"));

    QStringList ids = settings.getAllSettingIds();
    EXPECT_TRUE(ids.contains("test/adouble"));
    EXPECT_TRUE(ids.contains("test/another"));
}

TEST_F(TestSettings, restoreSettingsResetsOwnAndChildSettings)
{
    GtAbstractSettings child(&settings);
    settings.registerSetting("test/root", 11);
    child.registerSetting("test/child", 22);

    settings.setSetting("test/root", 99);
    child.setSetting("test/child", 77);

    settings.restoreSettings();

    EXPECT_EQ(settings.getSetting("test/root").toInt(), 11);
    EXPECT_EQ(child.getSetting("test/child").toInt(), 22);
}

/// check if module settings path actaully contains the correct name
TEST_F(TestSettings, moduleSettingsPath)
{
    QString value = gt::makeModuleSettingsPath("bla");
    ASSERT_TRUE(value.contains("Unittests"));
}

/// check if module settings path is correct (using a module id)
TEST_F(TestSettings, moduleSettingsPathWithModuleID)
{
    QString value = gt::makeModuleSettingsPath("mymodule", "bla");
    ASSERT_TRUE(value.contains("mymodule/bla"));
}

TEST_F(TestSettings, concreteSettingsRoundTripAndDefaults)
{
    GtSettings concrete;

    EXPECT_EQ(concrete.lastSession(), QString{"default"});
    EXPECT_EQ(concrete.lastPerspective(), QString{"default"});
    EXPECT_EQ(concrete.lastProject(), QString{});
    EXPECT_EQ(concrete.lastPath(), QDir::homePath());
    EXPECT_FALSE(concrete.openLastSession());
    EXPECT_FALSE(concrete.openLastProject());
    EXPECT_EQ(concrete.language(), QString{});
    EXPECT_TRUE(concrete.showStartupPage());
    EXPECT_TRUE(concrete.searchForUpdate());
    EXPECT_EQ(concrete.maxLogLength(), 2000);
    EXPECT_EQ(concrete.loggingVerbosity(), 0);
    EXPECT_FALSE(concrete.useExtendedProcessExecutor());
    EXPECT_FALSE(concrete.autostartProcessRunner());
    EXPECT_FALSE(concrete.firstApplicationRun());

    concrete.setLastSession("session-a");
    concrete.setLastProject("project-a");
    concrete.setLastPerspective("perspective-a");
    concrete.setLastPath("/tmp/path");
    concrete.setOpenLastSession(true);
    concrete.setOpenLastProject(true);
    concrete.setLanguage("de");
    concrete.setShowStartupPage(false);
    concrete.setSearchForUpdate(false);
    concrete.setMaxLogLength(1234);
    concrete.setLoggingVerbosity(7);
    concrete.setLoggingLevel(3);
    concrete.setUseExtendedProcessExecutor(true);
    concrete.setAutostartProcessRunner(true);

    EXPECT_EQ(concrete.lastSession(), QString{"session-a"});
    EXPECT_EQ(concrete.lastProject(), QString{"project-a"});
    EXPECT_EQ(concrete.lastPerspective(), QString{"perspective-a"});
    EXPECT_EQ(concrete.lastPath(), QString{"/tmp/path"});
    EXPECT_TRUE(concrete.openLastSession());
    EXPECT_TRUE(concrete.openLastProject());
    EXPECT_EQ(concrete.language(), QString{"de"});
    EXPECT_FALSE(concrete.showStartupPage());
    EXPECT_FALSE(concrete.searchForUpdate());
    EXPECT_EQ(concrete.maxLogLength(), 1234);
    EXPECT_EQ(concrete.loggingVerbosity(), 7);
    EXPECT_EQ(concrete.loggingLevel(), 3);
    EXPECT_TRUE(concrete.useExtendedProcessExecutor());
    EXPECT_TRUE(concrete.autostartProcessRunner());
}

TEST_F(TestSettings, shortcutsAndListsRoundTrip)
{
    GtSettings concrete;

    QList<GtShortCutSettingsData> shortcuts{{"id1", "cat1", "Ctrl+A", false},
                                            {"id2", "cat2", "Ctrl+B", true}};
    concrete.setShortcutsTable(shortcuts);

    auto restored = concrete.shortcutsList();
    ASSERT_EQ(restored.size(), 2);
    EXPECT_EQ(restored[0].id, QString{"id1"});
    EXPECT_EQ(restored[0].category, QString{"cat1"});
    EXPECT_EQ(restored[0].shortCut, QString{"Ctrl+A"});
    EXPECT_FALSE(restored[0].isReadOnly);
    EXPECT_EQ(restored[1].id, QString{"id2"});
    EXPECT_TRUE(restored[1].isReadOnly);

    concrete.setLastProcessElements(QStringList{"a", "b"});
    EXPECT_EQ(concrete.lastProcessElements(), QStringList({"a", "b"}));

    concrete.setLastProcessElements(QStringList{"1","2","3","4","5","6","7","8","9","10","11"});
    EXPECT_EQ(concrete.lastProcessElements(), QStringList({"a", "b"}));

    concrete.setExplorerExpandStates(QStringList{"x", "y"});
    EXPECT_EQ(concrete.explorerExpandStates(), QStringList({"x", "y"}));

    EXPECT_TRUE(concrete.intialShortCutsList().isEmpty());
    EXPECT_FALSE(concrete.initialShortCuts().isValid());
}
