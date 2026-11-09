/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_applicationpaths.h"

#include "gt_logging.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTemporaryFile>
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
                QStringLiteral("The system temporary location is unavailable.");
        }
        return false;
    }

    if (!QDir(tempRoot).exists())
    {
        if (error)
        {
            *error = QStringLiteral(
                         "The system temporary location does not exist: %1")
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
            *error = QStringLiteral("Could not create an application temporary "
                                    "directory: %1")
                         .arg(directory->errorString());
        }
        return false;
    }

    gtInfo() << "Using temporary directory:" << directory->path();
    owner.directory = std::move(directory);
    return true;
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
                *error = QStringLiteral(
                    "The application data location for state is unavailable.");
            }
            return {};
        }
        return QDir(dataPath).filePath(QStringLiteral("state"));
    }
#endif

    case GtApplicationPaths::Location::Runtime: {
        QString runtimeRoot =
            QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation);

        if (runtimeRoot.isEmpty())
        {
            const QString tempRoot =
                QStandardPaths::writableLocation(QStandardPaths::TempLocation);
            if (tempRoot.isEmpty())
            {
                if (error)
                {
                    *error = QStringLiteral(
                        "Neither runtime nor temporary location is available.");
                }
                return {};
            }

            const QByteArray userIdentity = QDir::homePath().toUtf8();
            const QByteArray userHash =
                QCryptographicHash::hash(userIdentity,
                                         QCryptographicHash::Sha256)
                    .toHex()
                    .left(16);
            runtimeRoot = QDir(tempRoot).filePath(
                QStringLiteral("gtlab-runtime-%1")
                    .arg(QString::fromLatin1(userHash)));
        }

        const QString organization = QCoreApplication::organizationName();
        const QString application = QCoreApplication::applicationName();
        return QDir(runtimeRoot)
            .filePath(QDir(organization).filePath(application));
    }

    case GtApplicationPaths::Location::Temp:
        break;
    }

    if (error)
    {
        *error = QStringLiteral("Unknown application path location.");
    }
    return {};
}

bool
ensureWritableDirectory(const QString& path, QString* error)
{
    if (path.isEmpty())
    {
        if (error && error->isEmpty())
        {
            *error =
                QStringLiteral("The requested writable location is empty.");
        }
        return false;
    }

    if (!QDir().mkpath(path))
    {
        if (error)
        {
            *error =
                QStringLiteral("Could not create directory '%1'.").arg(path);
        }
        return false;
    }

    QTemporaryFile writeProbe(
        QDir(path).filePath(QStringLiteral(".gtlab-write-test-XXXXXX")));
    if (!writeProbe.open())
    {
        if (error)
        {
            *error = QStringLiteral("Directory '%1' is not writable: %2")
                         .arg(path, writeProbe.errorString());
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
    if (!ensureWritableDirectory(path, error))
    {
        return std::nullopt;
    }

    return QDir(path);
}
