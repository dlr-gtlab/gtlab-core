/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_coreapplication.h"
#include "gt_finally.h"
#include "gt_migratecollectionsdata.h"
#include "gt_settings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLockFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>

namespace
{

const auto migrationTestRoot =
    QStringLiteral("GTLAB_COLLECTION_MIGRATION_TEST_ROOT");
const auto migrationTestMode =
    QStringLiteral("GTLAB_COLLECTION_MIGRATION_TEST_MODE");
const auto migrationTestDestination =
    QStringLiteral("GTLAB_COLLECTION_MIGRATION_TEST_DESTINATION");
const auto itemPath = QStringLiteral("group/item/index.json");

QString
destinationPath(const QDir& root)
{
    return root.filePath(QStringLiteral("destination/collections"));
}

class CollectionMigrationTest : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(directory.isValid());
        const QDir root(directory.path());
        oldPath = root.filePath(QStringLiteral("Collections"));
        newPath = destinationPath(root);
    }

    void migrate(const QString& mode = QStringLiteral("success"))
    {
        // The migration locates the legacy directory beside the executable
        // directory. Relocate the runner to provide an isolated installation.
        const QString executable = QCoreApplication::applicationFilePath();
        const QString bin = directory.filePath(QStringLiteral("bin"));
        const QString testExecutable =
            QDir(bin).filePath(QFileInfo(executable).fileName());
        ASSERT_TRUE(QDir().mkpath(bin));
        ASSERT_TRUE(QFile::copy(executable, testExecutable));

        auto environment = QProcessEnvironment::systemEnvironment();
        environment.insert(migrationTestRoot, directory.path());
        environment.insert(migrationTestMode, mode);
        environment.insert(migrationTestDestination, newPath);
#ifdef Q_OS_WIN
        const QString libraryDirectory =
            QDir::toNativeSeparators(QFileInfo(executable).absolutePath());
        environment.insert(QStringLiteral("PATH"),
                           libraryDirectory + QDir::listSeparator() +
                               environment.value(QStringLiteral("PATH")));
#endif
        QProcess process;
        process.setProcessEnvironment(environment);
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.start(
            testExecutable,
            {QStringLiteral("--gtest_also_run_disabled_tests"),
             QStringLiteral("--gtest_filter=CollectionMigrationProcess."
                            "DISABLED_Run")});
        ASSERT_TRUE(process.waitForStarted(5000))
            << qPrintable(process.errorString());
        if (!process.waitForFinished(15000))
        {
            process.kill();
            process.waitForFinished(5000);
            FAIL() << "Migration test process timed out";
        }
        const QByteArray output = process.readAll();
        ASSERT_EQ(QProcess::NormalExit, process.exitStatus())
            << output.constData();
        ASSERT_EQ(0, process.exitCode()) << output.constData();
    }

    static void writeFile(const QString& base, const QString& path,
                          const QByteArray& contents)
    {
        QFile file(QDir(base).filePath(path));
        ASSERT_TRUE(QDir().mkpath(QFileInfo(file).absolutePath()));
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));
        ASSERT_EQ(contents.size(), file.write(contents));
    }

    static void expectContents(const QString& base, const QString& path,
                               const QByteArray& contents)
    {
        QFile file(QDir(base).filePath(path));
        ASSERT_TRUE(file.open(QIODevice::ReadOnly));
        EXPECT_EQ(contents, file.readAll());
    }

    QTemporaryDir directory;
    QString oldPath;
    QString newPath;
};

} // namespace

// Runs in the temporary installation created by the migration tests.
TEST(CollectionMigrationProcess, DISABLED_Run)
{
    using namespace gt::detail::collectionmigration;
    const QString rootPath =
        qEnvironmentVariable("GTLAB_COLLECTION_MIGRATION_TEST_ROOT");
    const QString mode =
        qEnvironmentVariable("GTLAB_COLLECTION_MIGRATION_TEST_MODE");
    ASSERT_FALSE(rootPath.isEmpty());
    ASSERT_TRUE(mode == QStringLiteral("success") ||
                mode == QStringLiteral("failure") ||
                mode == QStringLiteral("locked"));
    QDir root(QCoreApplication::applicationDirPath());
    ASSERT_TRUE(root.cdUp());
    ASSERT_EQ(QDir(rootPath).canonicalPath(), root.canonicalPath());

    QStandardPaths::setTestModeEnabled(true);
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       root.filePath(QStringLiteral("settings")));
    GtCoreApplication application(QCoreApplication::instance(),
                                  GtCoreApplication::AppMode::Batch);
    QCoreApplication::setApplicationName(
        QStringLiteral("GTlabCollectionMigrationTest-") + root.dirName());
    const auto cleanup = gt::finally([]() noexcept {
        for (const auto location : {QStandardPaths::AppConfigLocation,
                                    QStandardPaths::AppDataLocation})
        {
            const QString path = QStandardPaths::writableLocation(location);
            QDir appDirectory(path);
            if (!path.isEmpty() &&
                appDirectory.dirName() == QCoreApplication::applicationName())
            {
                appDirectory.removeRecursively();
            }
        }
    });
    const QString destination =
        qEnvironmentVariable("GTLAB_COLLECTION_MIGRATION_TEST_DESTINATION");
    ASSERT_FALSE(destination.isEmpty());
    application.init();
    application.settings()->setUserCollectionsDir(destination);
    ASSERT_EQ(State::Pending, settingToState(*application.settings()));

    const auto legacy = legacyPath();
    ASSERT_TRUE(legacy.has_value());
    EXPECT_EQ(root.filePath(QStringLiteral("Collections")), *legacy);

    const auto lockInfo = lockFile();
    ASSERT_TRUE(lockInfo.has_value());
    EXPECT_EQ(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),
        lockInfo->absolutePath());

    const QString lockPath = lockInfo->absoluteFilePath();
    if (mode == QStringLiteral("locked"))
    {
        QLockFile lock(lockPath);
        ASSERT_TRUE(lock.tryLock(0));
        EXPECT_FALSE(runMigration());
        EXPECT_TRUE(QFileInfo::exists(lockPath));
        EXPECT_FALSE(QFileInfo::exists(destination));
        lock.unlock();
    }

    EXPECT_EQ(mode != QStringLiteral("failure"), runMigration());
    EXPECT_FALSE(QFileInfo::exists(lockPath));
    EXPECT_EQ(State::Pending, settingToState(*application.settings()));
}

TEST_F(CollectionMigrationTest, CopiesToMissingDestinationAndKeepsOriginal)
{
    const auto hiddenFile =
        QStringLiteral(".hidden-collection/item/index.json");
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "original"));
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, hiddenFile, "hidden"));
    ASSERT_FALSE(QDir(newPath).exists());

    ASSERT_NO_FATAL_FAILURE(migrate());

    expectContents(newPath, itemPath, "original");
    expectContents(newPath, hiddenFile, "hidden");
    expectContents(oldPath, itemPath, "original");
}

TEST_F(CollectionMigrationTest, CopiesToExistingEmptyDestination)
{
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "original"));
    ASSERT_TRUE(QDir().mkpath(newPath));

    ASSERT_NO_FATAL_FAILURE(migrate());

    expectContents(newPath, itemPath, "original");
}

TEST_F(CollectionMigrationTest, PreservesExistingDestinationFileOnCollision)
{
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "legacy"));
    ASSERT_NO_FATAL_FAILURE(writeFile(newPath, itemPath, "existing"));

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    expectContents(newPath, itemPath, "existing");
    expectContents(oldPath, itemPath, "legacy");
}

TEST_F(CollectionMigrationTest, RetriesAfterAnotherMigrationReleasesTheLock)
{
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "original"));

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("locked")));

    expectContents(newPath, itemPath, "original");
}

TEST_F(CollectionMigrationTest, FailsWhenLegacyDirectoryIsMissing)
{
    ASSERT_FALSE(QDir(oldPath).exists());

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    EXPECT_FALSE(QFileInfo::exists(newPath));
}

TEST_F(CollectionMigrationTest, RejectsLegacyDirectoryAsDestination)
{
    const QDir legacy(oldPath);
    ASSERT_TRUE(legacy.mkpath(QStringLiteral("empty")));
    newPath = oldPath;

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    EXPECT_EQ(QStringList{QStringLiteral("empty")},
              legacy.entryList(QDir::AllEntries | QDir::NoDotAndDotDot));
}

TEST_F(CollectionMigrationTest, RejectsMissingDestinationInsideLegacyDirectory)
{
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "original"));
    const QString nestedParent =
        QDir(oldPath).filePath(QStringLiteral("nested"));
    newPath = QDir(nestedParent).filePath(QStringLiteral("collections"));

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    EXPECT_FALSE(QFileInfo::exists(nestedParent));
    expectContents(oldPath, itemPath, "original");
}

#ifdef Q_OS_UNIX
TEST_F(CollectionMigrationTest,
       RejectsDestinationThroughSymlinkToLegacyDirectory)
{
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, itemPath, "original"));
    const QString alias = directory.filePath(QStringLiteral("legacy-alias"));
    ASSERT_TRUE(QFile::link(oldPath, alias));
    newPath = QDir(alias).filePath(QStringLiteral("nested/collections"));

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    EXPECT_FALSE(
        QFileInfo::exists(QDir(oldPath).filePath(QStringLiteral("nested"))));
    expectContents(oldPath, itemPath, "original");
}
#endif

TEST_F(CollectionMigrationTest, StopsOnFirstCopyFailure)
{
    const auto first = QStringLiteral("group/a.txt");
    const auto blocked = QStringLiteral("group/b.txt");
    const auto last = QStringLiteral("group/c.txt");
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, first, "first"));
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, blocked, "blocked"));
    ASSERT_NO_FATAL_FAILURE(writeFile(oldPath, last, "last"));
    ASSERT_TRUE(QDir(newPath).mkpath(blocked));

    ASSERT_NO_FATAL_FAILURE(migrate(QStringLiteral("failure")));

    expectContents(newPath, first, "first");
    EXPECT_TRUE(QFileInfo(QDir(newPath).filePath(blocked)).isDir());
    EXPECT_FALSE(QFileInfo::exists(QDir(newPath).filePath(last)));
    expectContents(oldPath, blocked, "blocked");
    expectContents(oldPath, last, "last");
}
