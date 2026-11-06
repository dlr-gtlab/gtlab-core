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

namespace
{

class CollectionPathTest : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(directory.isValid());
        gtApp->settings()->setUserCollectionsDir(directory.path());
    }

    void TearDown() override
    {
        gtApp->settings()->setUserCollectionsDir(QString{});
    }

    QTemporaryDir directory;
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

int
main(int argc, char** argv)
{
    QApplication qtApp(argc, argv);
    QStandardPaths::setTestModeEnabled(true);
    QTemporaryDir settingsDirectory;
    if (!settingsDirectory.isValid()) return 1;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       settingsDirectory.path());
    qputenv("GTLAB_MODULE_DIRS", QByteArray{});

    GtApplication application(&qtApp, false, GtCoreApplication::AppMode::Batch);
    QCoreApplication::setApplicationName("GTlabCollectionPathTests");
    application.init();
    application.loadModules();
    if (!application.loadSingleModule(
            QString::fromUtf8(COLLECTION_FIXTURE_PATH)))
    {
        return 1;
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
