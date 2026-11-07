/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_application.h"
#include "gt_collectionloader.h"
#include "gt_settings.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <memory>

namespace
{

class CollectionTestApplication : public GtApplication
{
public:
    CollectionTestApplication() :
        GtApplication(QCoreApplication::instance(), false,
                      GtCoreApplication::AppMode::Batch)
    { }

protected:
    bool initFirstRun() override
    {
        return true;
    }
};

class CollectionPathTest : public testing::Test
{
protected:
    void SetUp() override
    {
        previousApplicationName = QCoreApplication::applicationName();
        previousOrganizationName = QCoreApplication::organizationName();
        previousOrganizationDomain = QCoreApplication::organizationDomain();
        previousTestMode = QStandardPaths::isTestModeEnabled();
        previousFormat = QSettings::defaultFormat();
        previousModuleDirs = qgetenv("GTLAB_MODULE_DIRS");
        ASSERT_TRUE(directory.isValid());
        QStandardPaths::setTestModeEnabled(true);
        QSettings::setDefaultFormat(QSettings::IniFormat);
        qputenv("GTLAB_MODULE_DIRS", QByteArray{});

        application = std::make_unique<CollectionTestApplication>();
        QCoreApplication::setApplicationName(
            "GTlabCollectionPathTests-" +
            QFileInfo(directory.path()).fileName());
        application->init();
        application->loadModules();
        ASSERT_TRUE(application->loadSingleModule(
            QString::fromUtf8(COLLECTION_FIXTURE_PATH)));
        application->settings()->setUserCollectionsDir(directory.path());
    }

    void TearDown() override
    {
        if (application)
        {
            QSettings settings;
            const QString settingsFile = settings.fileName();
            settings.clear();
            settings.sync();
            QFile::remove(settingsFile);
            QDir(GtCoreApplication::roamingPath()).removeRecursively();
            application.reset();
        }
        QCoreApplication::setApplicationName(previousApplicationName);
        QCoreApplication::setOrganizationName(previousOrganizationName);
        QCoreApplication::setOrganizationDomain(previousOrganizationDomain);
        QStandardPaths::setTestModeEnabled(previousTestMode);
        QSettings::setDefaultFormat(previousFormat);
        if (previousModuleDirs.isNull())
            qunsetenv("GTLAB_MODULE_DIRS");
        else
            qputenv("GTLAB_MODULE_DIRS", previousModuleDirs);
    }

    QTemporaryDir directory;
    std::unique_ptr<CollectionTestApplication> application;
    QString previousApplicationName;
    QString previousOrganizationName;
    QString previousOrganizationDomain;
    QByteArray previousModuleDirs;
    bool previousTestMode = false;
    QSettings::Format previousFormat = QSettings::NativeFormat;
};

TEST_F(CollectionPathTest, UsesConfiguredDirectoryAndCreatesCollectionFolder)
{
    const QString root = directory.filePath("custom collections/nested");
    gtApp->settings()->setUserCollectionsDir(root);
    const QString expected = QDir(root).filePath("CollectionPathTest");
    ASSERT_FALSE(QFileInfo::exists(expected));

    GtCollectionLoader loader("CollectionPathTest");
    EXPECT_EQ(expected, loader.collectionPath());
    EXPECT_TRUE(QFileInfo(expected).isDir());
    EXPECT_EQ(expected, loader.collectionPath());
}

TEST_F(CollectionPathTest, ReadsSettingAgainAfterDirectoryChanges)
{
    GtCollectionLoader loader("CollectionPathTest");
    EXPECT_EQ(directory.filePath("CollectionPathTest"),
              loader.collectionPath());

    const QString changed = directory.filePath("changed");
    gtApp->settings()->setUserCollectionsDir(changed);

    EXPECT_EQ(QDir(changed).filePath("CollectionPathTest"),
              loader.collectionPath());
    EXPECT_TRUE(
        QFileInfo(QDir(changed).filePath("CollectionPathTest")).isDir());
}

TEST_F(CollectionPathTest, EmptySettingFallsBackToRoamingDirectory)
{
    gtApp->settings()->setUserCollectionsDir(QString{});
    const QString expected = QDir(GtCoreApplication::roamingPath())
                                 .filePath("collections/CollectionPathTest");

    GtCollectionLoader loader("CollectionPathTest");
    EXPECT_EQ(expected, loader.collectionPath());
    EXPECT_TRUE(QFileInfo(expected).isDir());

    // Remove only this fixture's collection, never the shared test config root.
    EXPECT_TRUE(QDir(expected).removeRecursively());
}

TEST_F(CollectionPathTest, ReturnsEmptyWhenConfiguredRootIsAFile)
{
    const QString root = directory.filePath("file");
    QFile file(root);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.close();
    gtApp->settings()->setUserCollectionsDir(root);

    GtCollectionLoader loader("CollectionPathTest");
    EXPECT_TRUE(loader.collectionPath().isEmpty());
}

} // namespace
