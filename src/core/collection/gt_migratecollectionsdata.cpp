/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_migratecollectionsdata.h"

#include "gt_collectionsinstallpath.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

#include <algorithm>

namespace
{

constexpr auto collectionEntries =
    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;

bool
isMissingOrEmptyDirectory(const QString& path)
{
    const QFileInfo info(path);
    if (info.isDir())
    {
        const QDir dir(path);
        return dir.isReadable() && dir.isEmpty(collectionEntries);
    }

    return !info.exists() && !info.isSymLink();
}

bool
copyCollectionTree(const QDir& source, const QDir& target)
{
    if (!source.isReadable() || !target.mkpath(QStringLiteral(".")))
    {
        return false;
    }

    const auto entries = source.entryInfoList(collectionEntries);
    return std::all_of(
        entries.cbegin(), entries.cend(), [&target](const QFileInfo& entry) {
            if (entry.isSymLink())
            {
                return false;
            }

            const QString output = target.filePath(entry.fileName());
            return entry.isDir()
                       ? copyCollectionTree(QDir(entry.absoluteFilePath()),
                                            QDir(output))
                       : entry.isFile() &&
                             QFile::copy(entry.absoluteFilePath(), output);
        });
}

} // namespace

bool
gt::detail::migrateCollectionData(const QString& oldPath,
                                  const QString& newPath)
{
    if (oldPath.isEmpty() || newPath.isEmpty())
    {
        return false;
    }

    const QString sourcePath = QDir(oldPath).canonicalPath();
    if (sourcePath.isEmpty() || !isMissingOrEmptyDirectory(newPath))
    {
        return false;
    }

    const QString canonicalTarget = QFileInfo(newPath).canonicalFilePath();
    const QString targetPath =
        canonicalTarget.isEmpty() ? newPath : canonicalTarget;
    const QDir source(sourcePath);
    QDir parent = QFileInfo(targetPath).absoluteDir();

    // Keep the staging directory outside the source tree.
    QDir ancestor = parent;
    while (!ancestor.exists())
    {
        const QDir next = QFileInfo(ancestor.absolutePath()).absoluteDir();
        if (next.absolutePath() == ancestor.absolutePath())
        {
            return false;
        }
        ancestor = next;
    }

    const QString relative = source.relativeFilePath(ancestor.canonicalPath());
    if (relative != QStringLiteral("..") &&
        !relative.startsWith(QStringLiteral("../")) &&
        !QDir::isAbsolutePath(relative))
    {
        return false;
    }

    if (!parent.mkpath(QStringLiteral(".")))
    {
        return false;
    }

    QTemporaryDir staging(
        parent.filePath(QStringLiteral(".collections-XXXXXX")));
    if (!staging.isValid())
    {
        return false;
    }

    const QDir staged(QDir(staging.path()).filePath(QStringLiteral("data")));
    if (!copyCollectionTree(source, staged) ||
        !isMissingOrEmptyDirectory(targetPath))
    {
        return false;
    }

    // rmdir succeeds only if the existing destination is still empty.
    if (QFileInfo(targetPath).isDir() && !parent.rmdir(targetPath))
    {
        return false;
    }

    return parent.rename(staged.absolutePath(), targetPath);
}

std::optional<gt::detail::CollectionMigrationPaths>
gt::detail::checkCollectionMigration(const QString& applicationDirPath)
{
    QDir installationDir(applicationDirPath);
    if (!installationDir.cdUp())
    {
        return std::nullopt;
    }

    const QDir legacy(installationDir.filePath(QStringLiteral("Collections")));
    if (!legacy.exists() ||
        legacy.isEmpty(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks))
    {
        return std::nullopt;
    }

    bool ok = false;
    const QString newPath = collectionsInstallPath(false, &ok);
    if (!ok || !isMissingOrEmptyDirectory(newPath))
    {
        return std::nullopt;
    }

    return CollectionMigrationPaths{legacy.absolutePath(), newPath};
}
