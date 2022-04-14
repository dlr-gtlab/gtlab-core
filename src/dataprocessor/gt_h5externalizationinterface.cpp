/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5externalizationinterface.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_h5externalizationinterface.h"

#ifdef GT_H5
#include "gt_logging.h"
#include "gt_externalizationmanager.h"

#include "gth5_file.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

const QString GtH5ExternalizationInterface::S_PROJECT_BASENAME =
        QStringLiteral("project.ext");
const QString GtH5ExternalizationInterface::S_BACKUP_SUFFIX =
        QStringLiteral("_backup");

static auto* s_h5Interface = GtH5ExternalizationInterface::instance();

GtH5ExternalizationInterface::GtH5ExternalizationInterface() = default;

GtH5ExternalizationInterface*
GtH5ExternalizationInterface::instance()
{
    static GtH5ExternalizationInterface self{};

    return &self;
}

void
GtH5ExternalizationInterface::onProjectLoaded(const QString& /*projectDir*/)
{
    // file path without suffix
    QString filePath{h5ProjectFilePath(false)};

    QString backupFilePath{h5ProjectFilePath(true)};

    if (!QFileInfo::exists(filePath))
    {
        gtDebug() << tr("No HDF5 file to backup!");
        return;
    }

    if (QFileInfo::exists(backupFilePath) && !QDir().remove(backupFilePath))
    {
        gtError() << tr("Failed to remove project backup file!")
                  << backupFilePath;
        return;
    }

    if (!QFile::copy(filePath, backupFilePath))
    {
        gtError() << tr("Creating project backup file failed!");
    }
}

QString
GtH5ExternalizationInterface::h5ProjectFilePath(bool useBackupFile) const
{
    QDir projectDir{gtExternalizationManager->projectDir()};

    if (!projectDir.exists())
    {
        return {};
    }

    QString name{S_PROJECT_BASENAME};

    if (useBackupFile)
    {
        name += S_BACKUP_SUFFIX;
    }

    return projectDir.absoluteFilePath(name + GtH5File::dotFileSuffix());
}
#endif
