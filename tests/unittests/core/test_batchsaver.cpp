/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#include <gtest/gtest.h>

#include "gt_batchsaver.h"

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>

void makeUTF8(QTextStream& stream)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    stream.setCodec("UTF-8");
#endif
}

// Helper: read the whole file into a QString
static QString
readFileToString(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    QTextStream ts(&f);
    makeUTF8(ts);
    return ts.readAll();
}

// Fixture with a temporary directory per test
class GtBatchSaverTest : public ::testing::Test
{
protected:
    QTemporaryDir tempDir;

    QString makePath(const QString& name) const
    {
        return tempDir.path() + QLatin1Char('/') + name;
    }

    void SetUp() override
    {
        ASSERT_TRUE(tempDir.isValid());
    }
};

// --------------------------------------------------------
// Single file: create new file, no previous backup
// --------------------------------------------------------
TEST_F(GtBatchSaverTest, SingleFileCreateSuccess)
{
    const QString filePath = makePath("file.txt");

    GtBatchSaver batch;
    batch.addBinary(filePath, QByteArray("hello"));

    ASSERT_TRUE(batch.commit()) << batch.errorString().toStdString();

    // New file exists and contains expected content
    EXPECT_TRUE(QFile::exists(filePath));
    EXPECT_EQ(readFileToString(filePath), QString("hello"));

    // No backup should exist (no previous file)
    EXPECT_FALSE(QFile::exists(filePath + "_backup"));
}

// --------------------------------------------------------
// Single file: overwrite existing file, backup is created
// --------------------------------------------------------
TEST_F(GtBatchSaverTest, SingleFileOverwriteCreatesBackup)
{
    const QString filePath = makePath("file.txt");
    const QString backupPath = filePath + "_backup";

    // Create original file with old content
    {
        QFile f(filePath);
        ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream ts(&f);
        makeUTF8(ts);
        ts << "old";
    }
    ASSERT_TRUE(QFile::exists(filePath));

    GtBatchSaver batch;
    batch.addBinary(filePath, QByteArray("new"));

    ASSERT_TRUE(batch.commit()) << batch.errorString().toStdString();

    // New content in main file
    EXPECT_EQ(readFileToString(filePath), QString("new"));

    // Backup file exists and contains old content
    EXPECT_TRUE(QFile::exists(backupPath));
    EXPECT_EQ(readFileToString(backupPath), QString("old"));
}

// --------------------------------------------------------
// Multi-file: all succeed, all updated and backups created
// --------------------------------------------------------
TEST_F(GtBatchSaverTest, MultiFileCommitAllOrNothingSuccess)
{
    const QString fileA = makePath("A.txt");
    const QString fileB = makePath("B.txt");
    const QString backupA = fileA + "_backup";
    const QString backupB = fileB + "_backup";

    // Create original files
    {
        QFile fa(fileA);
        ASSERT_TRUE(fa.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream tsa(&fa);
        makeUTF8(tsa);
        tsa << "A_old";
    }
    {
        QFile fb(fileB);
        ASSERT_TRUE(fb.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream tsb(&fb);
        makeUTF8(tsb);
        tsb << "B_old";
    }

    GtBatchSaver batch;
    batch.addBinary(fileA, QByteArray("A_new"));
    batch.addBinary(fileB, QByteArray("B_new"));

    ASSERT_TRUE(batch.commit()) << batch.errorString().toStdString();

    // New contents
    EXPECT_EQ(readFileToString(fileA), QString("A_new"));
    EXPECT_EQ(readFileToString(fileB), QString("B_new"));

    // Backups exist with old content
    EXPECT_TRUE(QFile::exists(backupA));
    EXPECT_TRUE(QFile::exists(backupB));
    EXPECT_EQ(readFileToString(backupA), QString("A_old"));
    EXPECT_EQ(readFileToString(backupB), QString("B_old"));
}

// --------------------------------------------------------
// Multi-file: one writer fails -> no file changes at all
// --------------------------------------------------------
TEST_F(GtBatchSaverTest, MultiFileCommitFailsNoPartialWrites)
{
    const QString fileA = makePath("A.txt");
    const QString fileB = makePath("B.txt");
    const QString backupA = fileA + "_backup";
    const QString backupB = fileB + "_backup";

    // Create original files
    {
        QFile fa(fileA);
        ASSERT_TRUE(fa.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream tsa(&fa);
        makeUTF8(tsa);
        tsa << "A_old";
    }
    {
        QFile fb(fileB);
        ASSERT_TRUE(fb.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream tsb(&fb);
        makeUTF8(tsb);
        tsb << "B_old";
    }

    GtBatchSaver batch;

    // A: normal writer
    batch.addBinary(fileA, QByteArray("A_new"));

    // B: failing writer (simulates error in serialization)
    batch.addOp(fileB, [](QIODevice& dev) -> bool {
        Q_UNUSED(dev);
        return false; // trigger failure
    });

    // Commit must fail
    EXPECT_FALSE(batch.commit());
    EXPECT_FALSE(batch.errorString().isEmpty());

    // Original files must be unchanged
    EXPECT_TRUE(QFile::exists(fileA));
    EXPECT_TRUE(QFile::exists(fileB));
    EXPECT_EQ(readFileToString(fileA), QString("A_old"));
    EXPECT_EQ(readFileToString(fileB), QString("B_old"));

    // No backups should have been created (nothing committed)
    EXPECT_FALSE(QFile::exists(backupA));
    EXPECT_FALSE(QFile::exists(backupB));
}

// --------------------------------------------------------
// addXml: basic XML writing via batch saver
// --------------------------------------------------------
TEST_F(GtBatchSaverTest, AddXmlWritesDomDocument)
{
    const QString filePath = makePath("doc.xml");

    QDomDocument doc;
    QDomElement root = doc.createElement("Root");
    doc.appendChild(root);
    QDomElement child = doc.createElement("Child");
    child.setAttribute("value", "42");
    root.appendChild(child);

    GtBatchSaver batch;
    batch.addXml(filePath, doc, true);

    ASSERT_TRUE(batch.commit()) << batch.errorString().toStdString();

    EXPECT_TRUE(QFile::exists(filePath));
    const QString content = readFileToString(filePath);

    // Very loose check: root and child tags are present
    EXPECT_NE(content.indexOf("<Root>"), -1);
    EXPECT_NE(content.indexOf("<Child"), -1);
    EXPECT_NE(content.indexOf("value=\"42\""), -1);
}
