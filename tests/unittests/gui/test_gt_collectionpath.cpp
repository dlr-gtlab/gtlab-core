/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_applicationpaths.h"
#include "gt_collectionsinstallpath.h"
#include "gt_coreapplication.h"
#include "gt_migratecollectionsdata.h"
#include "gt_settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <memory>

namespace
{

const QString collectionId = QStringLiteral("CollectionPathTest");

class CollectionTestApplication : public GtCoreApplication
{
public:
    CollectionTestApplication() :
        GtCoreApplication(QCoreApplication::instance(), AppMode::Batch)
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
        previousApplicationVersion = QCoreApplication::applicationVersion();
        previousOrganizationName = QCoreApplication::organizationName();
        previousOrganizationDomain = QCoreApplication::organizationDomain();
        previousTestMode = QStandardPaths::isTestModeEnabled();
        previousFormat = QSettings::defaultFormat();
        ASSERT_TRUE(directory.isValid());
        QStandardPaths::setTestModeEnabled(true);
        QSettings::setDefaultFormat(QSettings::IniFormat);

        application = std::make_unique<CollectionTestApplication>();
        QCoreApplication::setApplicationName(
            "GTlabCollectionPathTests-" +
            QFileInfo(directory.path()).fileName());
        application->init();
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
            application.reset();
        }
        QCoreApplication::setApplicationName(previousApplicationName);
        QCoreApplication::setApplicationVersion(previousApplicationVersion);
        QCoreApplication::setOrganizationName(previousOrganizationName);
        QCoreApplication::setOrganizationDomain(previousOrganizationDomain);
        QStandardPaths::setTestModeEnabled(previousTestMode);
        QSettings::setDefaultFormat(previousFormat);
    }

    QTemporaryDir directory;
    std::unique_ptr<CollectionTestApplication> application;
    QString previousApplicationName;
    QString previousApplicationVersion;
    QString previousOrganizationName;
    QString previousOrganizationDomain;
    bool previousTestMode = false;
    QSettings::Format previousFormat = QSettings::NativeFormat;
};

TEST_F(CollectionPathTest, UsesConfiguredDirectoryAndCreatesCollectionFolder)
{
    const QString root = directory.filePath("custom collections/nested");
    gtApp->settings()->setUserCollectionsDir(root);
    const QString expected = QDir(root).filePath(collectionId);
    ASSERT_FALSE(QFileInfo::exists(expected));

    EXPECT_EQ(expected, gt::detail::collectionInstallPath(collectionId));
    EXPECT_TRUE(QFileInfo(expected).isDir());
    EXPECT_EQ(expected, gt::detail::collectionInstallPath(collectionId));
}

TEST_F(CollectionPathTest, ReadsSettingAgainAfterDirectoryChanges)
{
    EXPECT_EQ(directory.filePath(collectionId),
              gt::detail::collectionInstallPath(collectionId));

    const QString changed = directory.filePath("changed");
    gtApp->settings()->setUserCollectionsDir(changed);

    const QString expected = QDir(changed).filePath(collectionId);
    EXPECT_EQ(expected, gt::detail::collectionInstallPath(collectionId));
    EXPECT_TRUE(QFileInfo(expected).isDir());
}

TEST_F(CollectionPathTest, EmptySettingFallsBackToDataDirectory)
{
    gtApp->settings()->setUserCollectionsDir(QString{});
    const auto dataDirectory =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Data);
    ASSERT_TRUE(dataDirectory.has_value());
    const QString expected =
        QDir(dataDirectory->filePath("Collections")).filePath(collectionId);

    EXPECT_EQ(expected, gt::detail::collectionInstallPath(collectionId));
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

    EXPECT_TRUE(gt::detail::collectionInstallPath(collectionId).isEmpty());
}

TEST_F(CollectionPathTest, PersistsMigrationStatesAndRestoresPending)
{
    using namespace gt::detail::collectionmigration;
    auto& settings = *application->settings();
    EXPECT_EQ(State::Pending, settingToState(settings));
    EXPECT_EQ(QStringLiteral("pending"), settings.collectionMigrationState());

    settings.setCollectionMigrationState(State::Suppressed);
    EXPECT_EQ(State::Suppressed, settingToState(GtSettings{}));
    EXPECT_EQ(QStringLiteral("suppressed"),
              GtSettings().collectionMigrationState());

    settings.restoreSettings();
    EXPECT_EQ(State::Pending, settingToState(GtSettings{}));

    settings.setCollectionMigrationState(State::Completed);
    EXPECT_EQ(State::Completed, settingToState(GtSettings{}));
    EXPECT_EQ(QStringLiteral("completed"),
              GtSettings().collectionMigrationState());

    settings.restoreSettings();
    EXPECT_EQ(State::Pending, settingToState(GtSettings{}));
    EXPECT_EQ(QStringLiteral("pending"),
              GtSettings().collectionMigrationState());
}

TEST_F(CollectionPathTest, UnrecognizedMigrationStateIsPending)
{
    const QString key = QStringLiteral("application/migration/collections");
    auto& settings = *application->settings();
    ASSERT_TRUE(settings.hasSetting(key));
    settings.setSetting(key, QStringLiteral("declined"));

    EXPECT_EQ(gt::detail::collectionmigration::State::Pending,
              gt::detail::collectionmigration::settingToState(settings));
}

} // namespace
