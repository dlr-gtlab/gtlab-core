/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_applicationpaths.h"
#include "gt_coreapplication.h"

#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QUuid>

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

QString
readTemporaryDirectoryLine(QProcess& process, const QString& mode)
{
    process.start(QCoreApplication::applicationFilePath(), {mode});
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

TEST(GtApplicationPaths, TemporaryDirectoryIsSharedAndProcessOwned)
{
    const auto first =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
    const auto second =
        GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
    ASSERT_TRUE(first);
    ASSERT_TRUE(second);
    EXPECT_EQ(first->absolutePath(), second->absolutePath());

    QProcess crashedProcess;
    const QString crashedPath = readTemporaryDirectoryLine(
        crashedProcess, QStringLiteral("--application-temp-dir-hold"));
    RemoveDirectoryOnExit cleanup{crashedPath};
    ASSERT_FALSE(crashedPath.isEmpty());
    ASSERT_TRUE(QDir(crashedPath).exists());

    QProcess normalProcess;
    const QString normalPath = readTemporaryDirectoryLine(
        normalProcess, QStringLiteral("--application-temp-dir-exit"));
    ASSERT_FALSE(normalPath.isEmpty());
    EXPECT_NE(first->absolutePath(), normalPath);
    EXPECT_NE(normalPath, crashedPath);
    ASSERT_TRUE(normalProcess.waitForFinished(5000));
    EXPECT_EQ(normalProcess.exitStatus(), QProcess::NormalExit);
    EXPECT_FALSE(QDir(normalPath).exists());
    EXPECT_TRUE(QDir(crashedPath).exists());

    crashedProcess.kill();
    ASSERT_TRUE(crashedProcess.waitForFinished(5000));
    EXPECT_TRUE(QDir(crashedPath).exists());

    QProcess anotherProcess;
    const QString anotherPath = readTemporaryDirectoryLine(
        anotherProcess, QStringLiteral("--application-temp-dir-exit"));
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
