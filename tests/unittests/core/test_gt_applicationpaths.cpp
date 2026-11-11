/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_applicationpaths.h"
#include "gt_coreapplication.h"
#include "internal/gt_coreapplication_p.h"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTextStream>
#include <QThread>
#include <QUuid>
#include <QtGlobal>

namespace
{

class ScopedTestPaths
{
public:
    ScopedTestPaths() :
        organization(QCoreApplication::organizationName()),
        application(QCoreApplication::applicationName())
    {
        QStandardPaths::setTestModeEnabled(true);
        QCoreApplication::setOrganizationName(QStringLiteral("GTlabTests"));
        QCoreApplication::setApplicationName(
            QStringLiteral("ApplicationPaths-%1")
                .arg(QUuid::createUuid().toString(QUuid::WithoutBraces)));
    }

    ~ScopedTestPaths()
    {
        for (const QString& path : paths)
        {
            QDir(path).removeRecursively();
        }
        QCoreApplication::setOrganizationName(organization);
        QCoreApplication::setApplicationName(application);
        QStandardPaths::setTestModeEnabled(false);
    }

    void addPath(const QString& path)
    {
        paths << path;
    }

private:
    QString organization;
    QString application;
    QStringList paths;
};

struct RemoveDirectoryOnExit
{
    QString path;

    ~RemoveDirectoryOnExit()
    {
        if (!path.isEmpty())
        {
            QDir(path).removeRecursively();
        }
    }
};

#ifdef Q_OS_UNIX
class ScopedEnvironmentVariable
{
public:
    explicit ScopedEnvironmentVariable(const char* variableName) :
        name(variableName),
        wasSet(qEnvironmentVariableIsSet(variableName)),
        previousValue(qgetenv(variableName))
    { }

    ~ScopedEnvironmentVariable()
    {
        if (wasSet)
        {
            qputenv(name, previousValue);
        }
        else
        {
            qunsetenv(name.constData());
        }
    }

    bool set(const QString& value)
    {
        return qputenv(name, value.toLocal8Bit());
    }

private:
    QByteArray name;
    bool wasSet;
    QByteArray previousValue;
};

bool
blockPreferredRuntimeLocation(const QString& temporaryRoot,
                              ScopedEnvironmentVariable& xdgRuntimeDirectory,
                              ScopedEnvironmentVariable& temporaryDirectory)
{
    const QString invalidRuntimePath =
        QDir(temporaryRoot).filePath(QStringLiteral("invalid-runtime"));
    QFile invalidRuntime(invalidRuntimePath);
    if (!invalidRuntime.open(QIODevice::WriteOnly))
    {
        return false;
    }
    invalidRuntime.close();

    if (!xdgRuntimeDirectory.set(invalidRuntimePath) ||
        !temporaryDirectory.set(temporaryRoot))
    {
        return false;
    }

    const QString runtimeRoot =
        QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (runtimeRoot.isEmpty())
    {
        return true;
    }

    const QString relativePath =
        QDir(temporaryRoot).relativeFilePath(runtimeRoot);
    if (relativePath == QStringLiteral("..") ||
        relativePath.startsWith(QStringLiteral("../")))
    {
        return false;
    }

    QDir(runtimeRoot).removeRecursively();
    QFile runtimeBlocker(runtimeRoot);
    if (!runtimeBlocker.open(QIODevice::WriteOnly))
    {
        return false;
    }
    runtimeBlocker.close();
    return true;
}
#endif

QString
readTemporaryDirectoryLine(QProcess& process, const QString& mode)
{
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("GTLAB_APPLICATION_TEMP_DIR_TEST_MODE"),
                       mode);
    process.setProcessEnvironment(environment);
    process.start(QCoreApplication::applicationFilePath(),
                  {QStringLiteral("--gtest_filter=GtApplicationPaths."
                                  "DISABLED_TemporaryDirectoryProcessHelper"),
                   QStringLiteral("--gtest_also_run_disabled_tests")});
    if (!process.waitForStarted(5000))
    {
        process.kill();
        process.waitForFinished(5000);
        return {};
    }

    while (process.waitForReadyRead(5000))
    {
        while (process.canReadLine())
        {
            const QString line =
                QString::fromLocal8Bit(process.readLine()).trimmed();
            if (line.startsWith(QStringLiteral("GTLAB_TEMP_PATH=")))
            {
                return line.mid(QStringLiteral("GTLAB_TEMP_PATH=").size());
            }
        }
    }

    process.kill();
    process.waitForFinished(5000);
    return {};
}

} // namespace

// Launched only as a child process by TemporaryDirectoryProcessLifecycle.
TEST(GtApplicationPaths, DISABLED_TemporaryDirectoryProcessHelper)
{
    const QString mode =
        qEnvironmentVariable("GTLAB_APPLICATION_TEMP_DIR_TEST_MODE");
    ASSERT_TRUE(mode == QStringLiteral("hold") ||
                mode == QStringLiteral("exit"));

    const auto directory =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
    ASSERT_TRUE(directory);

    QTextStream(stdout) << "GTLAB_TEMP_PATH=" << directory->absolutePath()
                        << Qt::endl;
    if (mode == QStringLiteral("hold"))
    {
        while (true)
        {
            QThread::sleep(1);
        }
    }
}

TEST(GtApplicationPaths, CreatesWritableStandardDirectories)
{
    ScopedTestPaths testPaths;
    const GtApplicationPaths::Location locations[] = {
        GtApplicationPaths::Location::Config,
        GtApplicationPaths::Location::Data, GtApplicationPaths::Location::Cache,
        GtApplicationPaths::Location::State};

    for (const auto location : locations)
    {
        QString error;
        const auto directory = GtApplicationPaths::directory(location, &error);
        ASSERT_TRUE(directory) << error.toStdString();
        EXPECT_TRUE(directory->exists());
        testPaths.addPath(directory->absolutePath());
    }
}

TEST(GtApplicationPaths, ReturnsErrorWhenDirectoryCannotBeCreated)
{
    ScopedTestPaths testPaths;
    const QString cachePath =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const QString parentPath = QFileInfo(cachePath).absolutePath();
    ASSERT_TRUE(QDir().mkpath(parentPath));
    QFile blocker(cachePath);
    ASSERT_TRUE(blocker.open(QIODevice::WriteOnly));
    blocker.close();

    QString error;
    const auto directory = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Cache, &error);
    EXPECT_FALSE(directory);
    EXPECT_FALSE(error.isEmpty());
    EXPECT_TRUE(QFile::remove(cachePath));
}

TEST(GtApplicationPaths, RejectsUnknownLocation)
{
    QString error;
    const auto directory = GtApplicationPaths::directory(
        static_cast<GtApplicationPaths::Location>(-1), &error);
    EXPECT_FALSE(directory);
    EXPECT_FALSE(error.isEmpty());
}

#ifdef Q_OS_UNIX
TEST(GtApplicationPaths, RuntimeUsesPreferredLocationWhenAvailable)
{
    ScopedTestPaths testPaths;
    QTemporaryDir runtimeRoot;
    ASSERT_TRUE(runtimeRoot.isValid());
    ScopedEnvironmentVariable runtimeDirectory("XDG_RUNTIME_DIR");
    ASSERT_TRUE(runtimeDirectory.set(runtimeRoot.path()));

    const QString expectedPath =
        QDir(runtimeRoot.path())
            .filePath(QDir(QCoreApplication::organizationName())
                          .filePath(QCoreApplication::applicationName()));

    QString error;
    const auto directory = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    ASSERT_TRUE(directory) << error.toStdString();
    EXPECT_TRUE(error.isEmpty());
    EXPECT_TRUE(directory->exists());
    EXPECT_EQ(directory->absolutePath(), expectedPath);
}

TEST(GtApplicationPaths, RuntimeFallsBackToStablePerUserLocation)
{
    ScopedTestPaths testPaths;
    QTemporaryDir runtimeTemp;
    ASSERT_TRUE(runtimeTemp.isValid());
    ScopedEnvironmentVariable xdgRuntimeDirectory("XDG_RUNTIME_DIR");
    ScopedEnvironmentVariable temporaryDirectory("TMPDIR");
    ASSERT_TRUE(blockPreferredRuntimeLocation(
        runtimeTemp.path(), xdgRuntimeDirectory, temporaryDirectory));

    const QString dataRoot =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    ASSERT_FALSE(dataRoot.isEmpty());
    testPaths.addPath(dataRoot);
    const QString expectedPath =
        QDir(dataRoot).filePath(QStringLiteral("runtime"));

    QString error;
    const auto first = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    ASSERT_TRUE(first) << error.toStdString();
    EXPECT_TRUE(error.isEmpty());
    EXPECT_TRUE(first->exists());
    EXPECT_EQ(first->absolutePath(), expectedPath);

    const auto second = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    ASSERT_TRUE(second) << error.toStdString();
    EXPECT_EQ(second->absolutePath(), first->absolutePath());
}

TEST(GtApplicationPaths, RuntimeFallbackRepairsAndReusesPrivateDirectory)
{
    ScopedTestPaths testPaths;
    QTemporaryDir runtimeTemp;
    ASSERT_TRUE(runtimeTemp.isValid());
    ScopedEnvironmentVariable xdgRuntimeDirectory("XDG_RUNTIME_DIR");
    ScopedEnvironmentVariable temporaryDirectory("TMPDIR");
    ASSERT_TRUE(blockPreferredRuntimeLocation(
        runtimeTemp.path(), xdgRuntimeDirectory, temporaryDirectory));

    const QString dataRoot =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    ASSERT_FALSE(dataRoot.isEmpty());
    testPaths.addPath(dataRoot);
    ASSERT_TRUE(QDir().mkpath(dataRoot));
    const QString fallbackPath =
        QDir(dataRoot).filePath(QStringLiteral("runtime"));
    ASSERT_TRUE(QDir().mkpath(fallbackPath));

    const QFileDevice::Permissions broadPermissions =
        QFileDevice::ReadOwner | QFileDevice::WriteOwner |
        QFileDevice::ExeOwner | QFileDevice::ReadGroup |
        QFileDevice::WriteGroup | QFileDevice::ExeGroup |
        QFileDevice::ReadOther | QFileDevice::WriteOther |
        QFileDevice::ExeOther;
    ASSERT_TRUE(QFile::setPermissions(fallbackPath, broadPermissions));

    QString error;
    const auto first = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    ASSERT_TRUE(first) << error.toStdString();
    EXPECT_TRUE(error.isEmpty());

    const auto second = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    ASSERT_TRUE(second) << error.toStdString();
    EXPECT_EQ(second->absolutePath(), first->absolutePath());

    const QFileDevice::Permissions permissions =
        QFile::permissions(fallbackPath);
    const QFileDevice::Permissions sharedPermissions =
        QFileDevice::ReadGroup | QFileDevice::WriteGroup |
        QFileDevice::ExeGroup | QFileDevice::ReadOther |
        QFileDevice::WriteOther | QFileDevice::ExeOther;
    EXPECT_EQ(permissions & sharedPermissions, QFileDevice::Permissions{});
}

TEST(GtApplicationPaths, RuntimeRejectsFileAtFallbackLocation)
{
    ScopedTestPaths testPaths;
    QTemporaryDir runtimeTemp;
    ASSERT_TRUE(runtimeTemp.isValid());
    ScopedEnvironmentVariable xdgRuntimeDirectory("XDG_RUNTIME_DIR");
    ScopedEnvironmentVariable temporaryDirectory("TMPDIR");
    ASSERT_TRUE(blockPreferredRuntimeLocation(
        runtimeTemp.path(), xdgRuntimeDirectory, temporaryDirectory));

    const QString dataRoot =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    ASSERT_FALSE(dataRoot.isEmpty());
    testPaths.addPath(dataRoot);
    ASSERT_TRUE(QDir().mkpath(dataRoot));
    const QString fallbackPath =
        QDir(dataRoot).filePath(QStringLiteral("runtime"));
    QDir(fallbackPath).removeRecursively();
    QFile::remove(fallbackPath);
    QFile fallbackBlocker(fallbackPath);
    ASSERT_TRUE(fallbackBlocker.open(QIODevice::WriteOnly));
    fallbackBlocker.close();

    QString error;
    const auto directory = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Runtime, &error);
    EXPECT_FALSE(directory);
    EXPECT_NE(error.indexOf(QStringLiteral("private fallback")), -1);
}
#endif

TEST(GtCoreApplicationPrivate, MigratesReadOnlyLegacyIni)
{
    QTemporaryDir sourceRoot;
    QTemporaryDir targetRoot;
    ASSERT_TRUE(sourceRoot.isValid());
    ASSERT_TRUE(targetRoot.isValid());

    const QString sourcePath =
        QDir(sourceRoot.path()).filePath(QStringLiteral("GTlab.ini"));
    const QString targetPath =
        QDir(targetRoot.path()).filePath(QStringLiteral("GTlab.ini"));
    const QByteArray contents("loading_crashed=LegacyModule\n");
    QFile source(sourcePath);
    ASSERT_TRUE(source.open(QIODevice::WriteOnly));
    ASSERT_EQ(source.write(contents), contents.size());
    source.close();
    ASSERT_TRUE(QFile::setPermissions(sourcePath, QFileDevice::ReadOwner));

    GtCoreApplicationPrivate::migrateLegacyApplicationIni(sourcePath,
                                                          targetPath);

    QFile target(targetPath);
    ASSERT_TRUE(target.open(QIODevice::ReadOnly));
    EXPECT_EQ(target.readAll(), contents);
    EXPECT_NE(QFile::permissions(targetPath) & QFileDevice::WriteOwner,
              QFileDevice::Permissions{});
}

TEST(GtCoreApplicationPrivate, DoesNotOverwriteExistingApplicationIni)
{
    QTemporaryDir sourceRoot;
    QTemporaryDir targetRoot;
    ASSERT_TRUE(sourceRoot.isValid());
    ASSERT_TRUE(targetRoot.isValid());

    const QString sourcePath =
        QDir(sourceRoot.path()).filePath(QStringLiteral("GTlab.ini"));
    const QString targetPath =
        QDir(targetRoot.path()).filePath(QStringLiteral("GTlab.ini"));
    QFile source(sourcePath);
    ASSERT_TRUE(source.open(QIODevice::WriteOnly));
    ASSERT_EQ(source.write("legacy state"), 12);
    source.close();
    QFile target(targetPath);
    ASSERT_TRUE(target.open(QIODevice::WriteOnly));
    ASSERT_EQ(target.write("current state"), 13);
    target.close();

    GtCoreApplicationPrivate::migrateLegacyApplicationIni(sourcePath,
                                                          targetPath);

    ASSERT_TRUE(target.open(QIODevice::ReadOnly));
    EXPECT_EQ(target.readAll(), QByteArray("current state"));
}

TEST(GtCoreApplicationPrivate, MissingLegacyIniIsNotCreated)
{
    QTemporaryDir sourceRoot;
    QTemporaryDir targetRoot;
    ASSERT_TRUE(sourceRoot.isValid());
    ASSERT_TRUE(targetRoot.isValid());

    const QString sourcePath =
        QDir(sourceRoot.path()).filePath(QStringLiteral("missing.ini"));
    const QString targetPath =
        QDir(targetRoot.path()).filePath(QStringLiteral("GTlab.ini"));

    GtCoreApplicationPrivate::migrateLegacyApplicationIni(sourcePath,
                                                          targetPath);

    EXPECT_FALSE(QFileInfo::exists(targetPath));
}

TEST(GtCoreApplicationPrivate, FailedMigrationKeepsLegacyIni)
{
    QTemporaryDir sourceRoot;
    QTemporaryDir targetRoot;
    ASSERT_TRUE(sourceRoot.isValid());
    ASSERT_TRUE(targetRoot.isValid());

    const QString sourcePath =
        QDir(sourceRoot.path()).filePath(QStringLiteral("GTlab.ini"));
    const QString targetPath =
        QDir(targetRoot.path()).filePath(QStringLiteral("missing/GTlab.ini"));
    QFile source(sourcePath);
    ASSERT_TRUE(source.open(QIODevice::WriteOnly));
    ASSERT_GT(source.write("legacy state"), 0);
    source.close();

    GtCoreApplicationPrivate::migrateLegacyApplicationIni(sourcePath,
                                                          targetPath);

    EXPECT_TRUE(QFileInfo::exists(sourcePath));
    EXPECT_FALSE(QFileInfo::exists(targetPath));
}

TEST(GtApplicationPaths, TemporaryDirectorySharedWithinProcess)
{
    const auto first =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
    const auto second =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
    ASSERT_TRUE(first);
    ASSERT_TRUE(second);
    EXPECT_EQ(first->absolutePath(), second->absolutePath());
}

TEST(GtApplicationPaths, TemporaryDirectoryProcessLifecycle)
{
    QProcess crashedProcess;
    const QString crashedPath =
        readTemporaryDirectoryLine(crashedProcess, QStringLiteral("hold"));
    RemoveDirectoryOnExit cleanup{crashedPath};
    ASSERT_FALSE(crashedPath.isEmpty());
    ASSERT_TRUE(QDir(crashedPath).exists());

    QProcess normalProcess;
    const QString normalPath =
        readTemporaryDirectoryLine(normalProcess, QStringLiteral("exit"));
    ASSERT_FALSE(normalPath.isEmpty());
    EXPECT_NE(normalPath, crashedPath);
    ASSERT_TRUE(normalProcess.waitForFinished(5000));
    EXPECT_EQ(normalProcess.exitStatus(), QProcess::NormalExit);
    EXPECT_FALSE(QDir(normalPath).exists());
    EXPECT_TRUE(QDir(crashedPath).exists());

    crashedProcess.kill();
    ASSERT_TRUE(crashedProcess.waitForFinished(5000));
    EXPECT_TRUE(QDir(crashedPath).exists());

    QProcess anotherProcess;
    const QString anotherPath =
        readTemporaryDirectoryLine(anotherProcess, QStringLiteral("exit"));
    ASSERT_FALSE(anotherPath.isEmpty());
    ASSERT_TRUE(anotherProcess.waitForFinished(5000));
    EXPECT_NE(normalPath, anotherPath);
    EXPECT_NE(crashedPath, anotherPath);
    EXPECT_TRUE(QDir(crashedPath).exists());
    EXPECT_FALSE(QDir(anotherPath).exists());
}

TEST(GtApplicationPaths, LegacyApplicationTempHelperStillCreatesFreshDirs)
{
    QDir first = GtCoreApplication::applicationTempDir();
    QDir second = GtCoreApplication::applicationTempDir();
    ASSERT_TRUE(first.exists());
    ASSERT_TRUE(second.exists());
    EXPECT_NE(first.absolutePath(), second.absolutePath());

    EXPECT_TRUE(GtCoreApplication::removeTempDir(first.absolutePath()));
    EXPECT_TRUE(GtCoreApplication::removeTempDir(second.absolutePath()));
}
