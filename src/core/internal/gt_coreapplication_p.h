/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#pragma once

#include "gt_logging.h"

#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QObject>
#include <QString>

namespace GtCoreApplicationPrivate
{

inline void
migrateLegacyApplicationIni(const QString& sourcePath,
                            const QString& targetPath)
{
    if (targetPath.isEmpty() || QFileInfo::exists(targetPath) ||
        !QFileInfo::exists(sourcePath))
    {
        return;
    }

    if (!QFile::copy(sourcePath, targetPath) && !QFileInfo::exists(targetPath))
    {
        gtWarning() << QObject::tr("Could not migrate application state from "
                                   "%1 to %2")
                           .arg(sourcePath, targetPath);
        return;
    }

    const QFileDevice::Permissions permissions = QFile::permissions(targetPath);
    if (!QFile::setPermissions(targetPath,
                               permissions | QFileDevice::WriteOwner))
    {
        gtWarning() << QObject::tr("Could not make migrated application state "
                                   "writable: %1")
                           .arg(targetPath);
    }
}

} // namespace GtCoreApplicationPrivate
