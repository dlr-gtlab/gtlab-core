/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_filesystem.cpp
 */

#include "gtest/gtest.h"

#include "gt_filesystem.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

/// This is a test fixture that does a init for each test
class TestGtFilesystem : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(sourceDir.isValid());
        ASSERT_TRUE(targetDir.isValid());

        writeFile(sourceDir.filePath("top.txt"), "top");
        writeFile(sourceDir.filePath("keep.dat"), "keep");

        QDir().mkpath(sourceDir.filePath("sub"));
        writeFile(sourceDir.filePath("sub/nested.txt"), "nested");
    }

    static void writeFile(const QString& fileName, const QString& content)
    {
        QFile file(fileName);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));

        QTextStream stream(&file);
        stream << content;
    }

    static QString readFile(const QString& fileName)
    {
        QFile file(fileName);
        EXPECT_TRUE(file.open(QIODevice::ReadOnly));
        return QString::fromUtf8(file.readAll());
    }

    QTemporaryDir sourceDir;
    QTemporaryDir targetDir;
};

TEST_F(TestGtFilesystem, directoryEntriesFiltersAndRecurses)
{
    auto files = gt::filesystem::directoryEntries(
        QDir(sourceDir.path()), true, QRegularExpression(".*\\.txt$"));

    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(files.contains("top.txt"));
    ASSERT_TRUE(files.contains("sub/nested.txt"));
}

TEST_F(TestGtFilesystem, directoryEntriesWithoutRecursionSkipsSubdirectories)
{
    auto files = gt::filesystem::directoryEntries(
        QDir(sourceDir.path()), false, QRegularExpression(".*"));

    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(files.contains("top.txt"));
    ASSERT_TRUE(files.contains("keep.dat"));
}

TEST_F(TestGtFilesystem, copyDirReturnsSrcNotExistsForMissingSource)
{
    auto status = gt::filesystem::copyDir(QDir(sourceDir.filePath("missing")),
                                          QDir(targetDir.path()));

    ASSERT_EQ(status, gt::filesystem::CopyStatus::SrcNotExists);
}

TEST_F(TestGtFilesystem, copyDirCopiesFilteredFilesRecursively)
{
    auto status = gt::filesystem::copyDir(
        QDir(sourceDir.path()),
        QDir(targetDir.path()),
        gt::filesystem::OverwriteFiles | gt::filesystem::Recursive,
        QRegularExpression(".*\\.txt$"));

    ASSERT_EQ(status, gt::filesystem::CopyStatus::Success);
    ASSERT_TRUE(QFileInfo::exists(targetDir.filePath("top.txt")));
    ASSERT_TRUE(QFileInfo::exists(targetDir.filePath("sub/nested.txt")));
    ASSERT_FALSE(QFileInfo::exists(targetDir.filePath("keep.dat")));
}

TEST_F(TestGtFilesystem, copyDirDoesNotOverwriteExistingFilesWithoutFlag)
{
    writeFile(targetDir.filePath("top.txt"), "old");

    auto status = gt::filesystem::copyDir(
        QDir(sourceDir.path()),
        QDir(targetDir.path()),
        gt::filesystem::Recursive,
        QRegularExpression("top\\.txt$"));

    ASSERT_EQ(status, gt::filesystem::CopyStatus::Success);
    ASSERT_EQ(readFile(targetDir.filePath("top.txt")), "old");
}

TEST_F(TestGtFilesystem, copyDirOverwritesExistingFilesWithFlag)
{
    writeFile(targetDir.filePath("top.txt"), "old");

    auto status = gt::filesystem::copyDir(
        QDir(sourceDir.path()),
        QDir(targetDir.path()),
        gt::filesystem::OverwriteFiles,
        QRegularExpression("top\\.txt$"));

    ASSERT_EQ(status, gt::filesystem::CopyStatus::Success);
    ASSERT_EQ(readFile(targetDir.filePath("top.txt")), "top");
}
