/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_applicationpaths.h"

#include "gt_logging.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QObject>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QtGlobal>

#include <memory>
#include <mutex>

namespace
{

struct TemporaryDirectoryOwner
{
    std::unique_ptr<QTemporaryDir> directory;
};

TemporaryDirectoryOwner&
applicationTemporaryDirectory()
{
    static TemporaryDirectoryOwner owner;
    return owner;
}

bool
createApplicationTemporaryDirectory(TemporaryDirectoryOwner& owner,
                                    QString* error)
{
    const QString tempRoot =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    if (tempRoot.isEmpty())
    {
        if (error)
        {
            *error =
                QObject::tr("The system temporary location is unavailable.");
        }
        return false;
    }

    if (!QDir(tempRoot).exists())
    {
        if (error)
        {
            *error =
                QObject::tr("The system temporary location does not exist: %1")
                    .arg(tempRoot);
        }
        return false;
    }

    auto directory = std::make_unique<QTemporaryDir>(
        QDir(tempRoot).filePath(QStringLiteral("gtlab-XXXXXX")));

    if (!directory->isValid())
    {
        if (error)
        {
            *error = QObject::tr("Could not create an application temporary "
                                 "directory: %1")
                         .arg(directory->errorString());
        }
        return false;
    }

    gtInfo() << QObject::tr("Using temporary directory:") << directory->path();
    owner.directory = std::move(directory);
    return true;
}

bool
ensurePrivateRuntimeDirectory(const QString& path)
{
    if (!QDir().mkpath(path))
    {
        return false;
    }

    QFileInfo info(path);
    if (info.isSymLink() || !info.isDir())
    {
        return false;
    }

#ifdef Q_OS_UNIX
    const QFileDevice::Permissions privatePermissions =
        QFileDevice::ReadOwner | QFileDevice::WriteOwner |
        QFileDevice::ExeOwner;
    const QFileDevice::Permissions sharedPermissions =
        QFileDevice::ReadGroup | QFileDevice::WriteGroup |
        QFileDevice::ExeGroup | QFileDevice::ReadOther |
        QFileDevice::WriteOther | QFileDevice::ExeOther;

    if (!QFile::setPermissions(path, privatePermissions))
    {
        return false;
    }

    info.refresh();
    const QFileDevice::Permissions permissions = info.permissions();
    return !info.isSymLink() && info.isDir() &&
           (permissions & privatePermissions) == privatePermissions &&
           (permissions & sharedPermissions) == QFileDevice::Permissions{} &&
           info.isWritable();
#else
    // AppLocalDataLocation is per-user; Windows ACLs come from the profile.
    return info.isWritable();
#endif
}

QString
runtimeDirectoryPath(QString* error)
{
    const QString organization = QCoreApplication::organizationName();
    const QString application = QCoreApplication::applicationName();
    const auto applicationPath = [&](const QString& root) {
        return QDir(root).filePath(QDir(organization).filePath(application));
    };

    const QString preferredRoot =
        QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);
    if (!preferredRoot.isEmpty())
    {
        const QString preferredPath = applicationPath(preferredRoot);
        if (QDir().mkpath(preferredPath) &&
            QFileInfo(preferredPath).isWritable())
        {
            return preferredPath;
        }
    }

    // Keep the fallback per-user and stable for cooperating processes.
    const QString fallbackRoot =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (fallbackRoot.isEmpty())
    {
        if (error)
        {
            *error = QObject::tr("Neither runtime nor per-user application "
                                 "data location is available.");
        }
        return {};
    }

    const QString fallbackPath =
        QDir(fallbackRoot).filePath(QStringLiteral("runtime"));
    if (ensurePrivateRuntimeDirectory(fallbackPath))
    {
        return fallbackPath;
    }

    if (error)
    {
        *error = QObject::tr("Runtime location '%1' is unusable and a private "
                             "fallback could not be created at '%2'.")
                     .arg(preferredRoot, fallbackPath);
    }
    return {};
}

QString
locationPath(GtApplicationPaths::Location type, QString* error)
{
    switch (type)
    {
    case GtApplicationPaths::Location::Config:
        return QStandardPaths::writableLocation(
            QStandardPaths::AppConfigLocation);

    case GtApplicationPaths::Location::Data:
        return QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

    case GtApplicationPaths::Location::Cache:
        return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    case GtApplicationPaths::Location::State:
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
        return QStandardPaths::writableLocation(QStandardPaths::StateLocation);
#else
    {
        const QString dataPath = QStandardPaths::writableLocation(
            QStandardPaths::AppLocalDataLocation);
        if (dataPath.isEmpty())
        {
            if (error)
            {
                *error = QObject::tr(
                    "The application data location for state is unavailable.");
            }
            return {};
        }
        return QDir(dataPath).filePath(QStringLiteral("state"));
    }
#endif

    case GtApplicationPaths::Location::Runtime:
        return runtimeDirectoryPath(error);

    case GtApplicationPaths::Location::Temp:
        break;
    }

    if (error)
    {
        *error = QObject::tr("Unknown application path location.");
    }
    return {};
}

bool
ensureDirectory(const QString& path, QString* error)
{
    if (path.isEmpty())
    {
        if (error && error->isEmpty())
        {
            *error = QObject::tr("The requested writable location is empty.");
        }
        return false;
    }

    if (!QDir().mkpath(path))
    {
        if (error)
        {
            *error = QObject::tr("Could not create directory '%1'.").arg(path);
        }
        return false;
    }
    return true;
}

} // namespace

std::optional<QDir>
GtApplicationPaths::directory(Location type, QString* error)
{
    if (error)
    {
        error->clear();
    }

    if (type == Location::Temp)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> guard(mutex);

        TemporaryDirectoryOwner& owner = applicationTemporaryDirectory();
        if (!owner.directory &&
            !createApplicationTemporaryDirectory(owner, error))
        {
            return std::nullopt;
        }

        return QDir(owner.directory->path());
    }

    const QString path = locationPath(type, error);
    if (!ensureDirectory(path, error))
    {
        return std::nullopt;
    }

    return QDir(path);
}
