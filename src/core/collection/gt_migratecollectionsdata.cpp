/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_migratecollectionsdata.h"

#include "gt_collectionsinstallpath.h"

#include "gt_applicationpaths.h"
#include "gt_settings.h"
#include "gt_finally.h"
#include "gt_logging.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLockFile>
#include <QObject>
#include <QSettings>
#include <QCoreApplication>

#include <algorithm>


namespace gt
{
namespace detail
{
namespace collectionmigration
{
QString
stateToString(State state)
{
    switch (state)
    {
    case State::Completed:
        return QStringLiteral("completed");
    case State::Suppressed:
        return QStringLiteral("suppressed");
    case State::Pending:
        return QStringLiteral("pending");
    }
    return QStringLiteral("pending");
}

State
stringToState(const QString& state)
{
    if (state == "completed") return State::Completed;
    if (state == "suppressed") return State::Suppressed;
    return State::Pending;
}

State
settingToState(const GtSettings& appSettings)
{
    return stringToState(appSettings.collectionMigrationState());
}

const std::optional<QFileInfo>
lockFile()
{
    auto configpath =
        GtApplicationPaths().directory(GtApplicationPaths::Location::Config);
    if (configpath.has_value())
        return QFileInfo(configpath.value().absoluteFilePath(
            QStringLiteral(".collection-migraton.lock")));
    else
        return {};
}

std::optional<QString>
legacyPath()
{
    QDir installationDir(QCoreApplication::applicationDirPath());

    if (!installationDir.cdUp())
    {
        gtError() << "Legacy path is invalid, cannot cdUp installation dir:"
                  << installationDir;
        return {};
    }

    QString legacy(
        installationDir.absoluteFilePath(QStringLiteral("Collections")));

    return legacy;
}

QString
canonicalMigrationSource(const QString& sourcePath)
{
    const QFileInfo sourceInfo(sourcePath);
    if (sourceInfo.isSymLink() || !sourceInfo.isDir()) return {};

    const QString source = sourceInfo.canonicalFilePath();
    const QString parent = sourceInfo.absoluteDir().canonicalPath();
    if (source.isEmpty() || parent.isEmpty()) return {};

    // Resolve installation path aliases without allowing the source itself
    // to redirect to a different directory.
    const QString expected = QDir(parent).filePath(sourceInfo.fileName());
#ifdef Q_OS_WIN
    if (source.compare(expected, Qt::CaseInsensitive) != 0) return {};
#else
    if (source != expected) return {};
#endif
    return source;
}

bool
isMigrationDestinationSafe(const QString& sourcePath,
                           const QString& destinationPath)
{
    if (destinationPath.isEmpty()) return false;

    const QString source = canonicalMigrationSource(sourcePath);
    if (source.isEmpty()) return false;

    // The destination may be missing. Resolve its nearest existing ancestor
    // so a symbolic link in the path cannot hide an overlap.
    QFileInfo ancestor(QDir(destinationPath).absolutePath());
    while (!ancestor.exists() && !ancestor.isSymLink())
    {
        const QString parent = ancestor.absolutePath();
        if (parent == ancestor.absoluteFilePath()) return false;
        ancestor.setFile(parent);
    }

    const QString resolvedAncestor = ancestor.canonicalFilePath();
    if (resolvedAncestor.isEmpty()) return false;

    const QString relative = QDir(source).relativeFilePath(resolvedAncestor);
    return relative == QStringLiteral("..") ||
           relative.startsWith(QStringLiteral("../")) ||
           QDir::isAbsolutePath(relative);
}

namespace
{
bool
copyDirectory(const QString& sourcePath, const QString& destinationPath)
{
    QDir sourceDir(sourcePath);

    if (!isMigrationDestinationSafe(sourcePath, destinationPath))
    {
        gtError() << QObject::tr(
                         "Cannot migrate collections from %1 to %2. "
                         "The source must be a directory at its original location. "
                         "The destination must be outside the source folder.")
                         .arg(QDir::toNativeSeparators(sourcePath),
                              QDir::toNativeSeparators(destinationPath));
        return false;
    }

    if (!QDir().mkpath(destinationPath))
    {
        gtError() << QObject::tr("Could not create directory %1 to copy %2.")
                         .arg(QDir::toNativeSeparators(destinationPath),
                              QDir::toNativeSeparators(sourcePath));
        return false;
    }

    const QFileInfoList entries =
        sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::Hidden |
                                QDir::System | QDir::NoDotAndDotDot);

    return std::all_of(
        entries.cbegin(), entries.cend(),
        [&destinationPath](const QFileInfo& entry) {
            const QString source = entry.absoluteFilePath();
            const QString destination =
                QDir(destinationPath).filePath(entry.fileName());
            if (entry.isSymLink())
            {
                gtError() << QObject::tr("Could not copy %1 to %2. "
                                         "Symbolic links are not supported.")
                                 .arg(QDir::toNativeSeparators(source),
                                      QDir::toNativeSeparators(destination));
                return false;
            }
            if (entry.isDir())
            {
                return copyDirectory(source, destination);
            }
            QFile file(source);
            if (file.copy(destination)) return true;

            gtError() << QObject::tr("Could not copy %1 to %2: %3")
                             .arg(QDir::toNativeSeparators(source),
                                  QDir::toNativeSeparators(destination),
                                  file.errorString());
            return false;
        });
}

} // namespace


bool
runMigration(QString* migratedDestination)
{
    auto lock1 = lockFile();
    if (!lock1.has_value())
    {
        gtError() << "Could not retrieve lock file path";
        return false;
    }

    QLockFile lock(lock1.value().absoluteFilePath());
    lock.setStaleLockTime(0);

    if (!lock.tryLock(1000))
    {
        gtError() << "Could not retrieve lock";
        return false;
    }

    auto unlockGuard = gt::finally([&lock] { lock.unlock(); });

    auto sourcePathRet = legacyPath();
    if (!sourcePathRet.has_value())
    {
        return false;
    }
    auto sourcePath = sourcePathRet.value();

    bool ok1;
    auto destination = collectionsInstallPath(false, &ok1);
    if (!ok1)
    {
        return false;
    }

    if (!copyDirectory(sourcePath, destination))
    {
        return false;
    }
    if (migratedDestination)
    {
        *migratedDestination = destination;
    }
    return true;
}


} // namespace collectionmigration
} // namespace detail
} // namespace gt
