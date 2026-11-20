/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_migratecollectionsdata.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>

namespace
{

class CollectionMigrationTest : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(directory.isValid());
        oldPath = directory.filePath("legacy");
        newPath = directory.filePath("destination/collections");
        ASSERT_TRUE(QDir(oldPath).mkpath("group/item"));
        ASSERT_NO_FATAL_FAILURE(writeFile(oldPath + itemPath, "original"));
    }

    static void writeFile(const QString& path, const QByteArray& contents)
    {
        QFile file(path);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));
        ASSERT_EQ(contents.size(), file.write(contents));
    }

    static void expectContents(const QString& path, const QByteArray& contents)
    {
        QFile file(path);
        ASSERT_TRUE(file.open(QIODevice::ReadOnly));
        EXPECT_EQ(contents, file.readAll());
    }

    QTemporaryDir directory;
    QString oldPath;
    QString newPath;
    const QString itemPath = QStringLiteral("/group/item/index.json");
};

TEST_F(CollectionMigrationTest, CopiesToMissingDestinationAndKeepsOriginal)
{
    ASSERT_FALSE(QDir(newPath).exists());
    ASSERT_TRUE(gt::detail::migrateCollectionData(oldPath, newPath));
    expectContents(newPath + itemPath, "original");
    expectContents(oldPath + itemPath, "original");
}

TEST_F(CollectionMigrationTest, CopiesToExistingEmptyDestination)
{
    ASSERT_TRUE(QDir().mkpath(newPath));
    ASSERT_TRUE(gt::detail::migrateCollectionData(oldPath, newPath));
    expectContents(newPath + itemPath, "original");
}

TEST_F(CollectionMigrationTest, PreservesPopulatedDestination)
{
    ASSERT_TRUE(QDir(newPath).mkpath("group/item"));
    ASSERT_NO_FATAL_FAILURE(writeFile(newPath + itemPath, "existing"));

    EXPECT_FALSE(gt::detail::migrateCollectionData(oldPath, newPath));
    expectContents(newPath + itemPath, "existing");
    expectContents(oldPath + itemPath, "original");
}

#ifdef Q_OS_UNIX
TEST_F(CollectionMigrationTest, FailedCopyLeavesDestinationEmptyAndCanBeRetried)
{
    ASSERT_TRUE(QDir().mkpath(newPath));
    const QString linkPath =
        oldPath + QStringLiteral("/group/item/linked.json");
    ASSERT_TRUE(QFile::link(oldPath + itemPath, linkPath));

    EXPECT_FALSE(gt::detail::migrateCollectionData(oldPath, newPath));
    EXPECT_TRUE(QDir(newPath).exists());
    EXPECT_TRUE(QDir(newPath).isEmpty());
    expectContents(oldPath + itemPath, "original");

    ASSERT_TRUE(QFile::remove(linkPath));
    ASSERT_TRUE(gt::detail::migrateCollectionData(oldPath, newPath));
    expectContents(newPath + itemPath, "original");
}
#endif

} // namespace
