/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#include "gt_batchsaver.h"

#include <gt_xmlutilities.h>

#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include <functional>

// -----------------------------------------------------------------------------
// Internal helpers (file-local)
// -----------------------------------------------------------------------------
namespace
{

    struct TargetFile
    {
        QString targetPath;
        QString backupPath;
        QString previousBackupPath;
    };

    void cancelAll(std::vector<std::unique_ptr<QSaveFile>>& saveFiles)
    {
        for (auto& sf : saveFiles)
        {
            if (sf)
                sf->cancelWriting();
        }
    }

    inline void setError(QString& lastError, const QString& msg)
    {
        lastError = msg;
    }

    bool prepareSaveFiles(const std::vector<GtBatchSaver::Op>& ops,
                          std::vector<TargetFile>& targets,
                          std::vector<std::unique_ptr<QSaveFile>>& saveFiles,
                          QString& lastError)
    {
        targets.clear();
        saveFiles.clear();

        targets.reserve(ops.size());
        saveFiles.reserve(ops.size());

        for (const auto& op : ops)
        {
            QFileInfo info(op.targetPath);
            QDir dir = info.dir();
            if (!dir.exists() && !dir.mkpath(QStringLiteral(".")))
            {
                setError(lastError,
                         QObject::tr("Cannot create directory for '%1'")
                             .arg(op.targetPath));
                cancelAll(saveFiles);
                return false;
            }

            auto sf = std::make_unique<QSaveFile>(op.targetPath);
            if (!sf->open(op.openMode))
            {
                setError(lastError,
                         QObject::tr("Cannot open QSaveFile for '%1'")
                             .arg(op.targetPath));
                cancelAll(saveFiles);
                return false;
            }

            if (!op.writer(*sf))
            {
                setError(lastError,
                         QObject::tr("Writer callback failed for '%1'")
                             .arg(op.targetPath));
                sf->cancelWriting();
                cancelAll(saveFiles);
                return false;
            }

            sf->flush();
            if (sf->error() != QFile::NoError)
            {
                setError(lastError,
                         QObject::tr("Error during flush for '%1' (code %2)")
                             .arg(op.targetPath)
                             .arg(sf->error()));
                sf->cancelWriting();
                cancelAll(saveFiles);
                return false;
            }

            TargetFile fileLocation;
            fileLocation.targetPath = op.targetPath;
            fileLocation.backupPath = op.targetPath + QStringLiteral("_backup");
            targets.push_back(std::move(fileLocation));
            saveFiles.push_back(std::move(sf));
        }

        return true;
    }

    bool renameOriginalsToBackups(
        std::vector<TargetFile>& files,
        std::vector<std::unique_ptr<QSaveFile>>& saveFiles, QString& lastError)
    {
        std::vector<TargetFile*> renamedFiles;
        std::vector<TargetFile*> movedBackups;
        renamedFiles.reserve(files.size());
        movedBackups.reserve(files.size());

        auto rollbackRenames = [&]() {
            for (auto it = renamedFiles.rbegin(); it != renamedFiles.rend(); ++it)
            {
                TargetFile* file = *it;
                if (QFile::exists(file->backupPath) &&
                    !QFile::exists(file->targetPath))
                {
                    QFile::rename(file->backupPath,
                                  file->targetPath); // best effort
                }
            }

            for (auto it = movedBackups.rbegin(); it != movedBackups.rend(); ++it)
            {
                TargetFile* file = *it;
                if (QFile::exists(file->previousBackupPath) &&
                    !QFile::exists(file->backupPath))
                {
                    QFile::rename(file->previousBackupPath,
                                  file->backupPath); // best effort
                }
            }
        };

        for (TargetFile& targetFile : files)
        {
            if (QFile::exists(targetFile.backupPath))
            {
                targetFile.previousBackupPath =
                    targetFile.backupPath + QStringLiteral(".previous");

                if (QFile::exists(targetFile.previousBackupPath))
                {
                    QFile::remove(targetFile.previousBackupPath);
                }

                if (!QFile::rename(targetFile.backupPath,
                                  targetFile.previousBackupPath))
                {
                    setError(lastError,
                             QObject::tr(
                                 "Failed to move existing backup aside for '%1'")
                                 .arg(targetFile.targetPath));
                    rollbackRenames();
                    cancelAll(saveFiles);
                    return false;
                }

                movedBackups.push_back(&targetFile);
            }

            // If there is no original file, nothing to rename for this file
            if (!QFile::exists(targetFile.targetPath))
                continue;

            if (!QFile::rename(targetFile.targetPath, targetFile.backupPath))
            {
                setError(lastError,
                         QObject::tr(
                             "Failed to rename original to backup for '%1'")
                             .arg(targetFile.targetPath));

                rollbackRenames();
                cancelAll(saveFiles);
                return false;
            }

            renamedFiles.push_back(&targetFile);
        }

        return true;
    }

    bool commitTempFiles(std::vector<TargetFile>& files,
                         std::vector<std::unique_ptr<QSaveFile>>& saveFiles,
                         QString& lastError)
    {
        bool ok = true;

        assert(files.size() == saveFiles.size());

        for (int i = 0; i < files.size(); ++i)
        {
            auto& sf = saveFiles[i];
            if (!sf->commit())
            {
                setError(lastError,
                         QObject::tr("QSaveFile commit failed for '%1'")
                             .arg(files[i].targetPath));
                ok = false;
                break;
            }
        }

        if (!ok)
        {
            // Best-effort rollback: remove new files and restore backups.
            for (auto& m : files)
            {
                if (QFile::exists(m.targetPath))
                {
                    QFile::remove(m.targetPath);
                }
                if (QFile::exists(m.backupPath))
                {
                    QFile::rename(m.backupPath, m.targetPath);
                }
                if (QFile::exists(m.previousBackupPath) &&
                    !QFile::exists(m.backupPath))
                {
                    QFile::rename(m.previousBackupPath, m.backupPath);
                }
            }
            return false;
        }

        for (auto& m : files)
        {
            if (QFile::exists(m.previousBackupPath))
            {
                QFile::remove(m.previousBackupPath);
            }
        }

        return true;
    }

} // unnamed namespace

// -----------------------------------------------------------------------------
// GtBatchSaver methods
// -----------------------------------------------------------------------------

void
GtBatchSaver::addOp(const QString& targetPath,
                    const std::function<bool(QIODevice&)>& writer,
                    QIODevice::OpenMode openMode)
{
    Op op;
    op.targetPath = targetPath;
    op.openMode = openMode;
    op.writer = writer;
    m_ops.push_back(std::move(op));
}

void
GtBatchSaver::addXml(const QString& targetPath, const QDomDocument& doc,
                         bool ordered)
{
    addOp(targetPath, [doc, ordered](QIODevice& dev) -> bool {
        return gt::xml::writeDomDocumentToDevice(dev, doc, ordered);
    });
}

void
GtBatchSaver::addBinary(const QString& targetPath, const QByteArray& data)
{
    addOp(targetPath, [data](QIODevice& dev) -> bool {
        const qint64 written = dev.write(data);
        return written == data.size();
    }, QIODevice::WriteOnly);
}

bool
GtBatchSaver::commit()
{
    m_lastError.clear();

    if (m_ops.empty())
        return true;

    std::vector<TargetFile> files;
    std::vector<std::unique_ptr<QSaveFile>> saveFiles;

    if (!prepareSaveFiles(m_ops, files, saveFiles, m_lastError))
        return false;

    if (!renameOriginalsToBackups(files, saveFiles, m_lastError))
        return false;

    return commitTempFiles(files, saveFiles, m_lastError);
}

QString
GtBatchSaver::errorString() const
{
    return m_lastError;
}
