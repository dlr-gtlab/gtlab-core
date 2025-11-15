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

    struct Meta
    {
        QString targetPath;
        QString backupPath;
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

    bool prepareSaveFiles(const QVector<GtBatchSaver::Op>& ops,
                          QVector<Meta>& metas,
                          std::vector<std::unique_ptr<QSaveFile>>& saveFiles,
                          QString& lastError)
    {
        metas.clear();
        saveFiles.clear();

        metas.reserve(ops.size());
        saveFiles.reserve(ops.size());

        for (const auto& op : ops)
        {
            QFileInfo info(op.targetPath);
            QDir dir = info.dir();
            if (!dir.exists() && !dir.mkpath(QStringLiteral(".")))
            {
                setError(lastError,
                         QStringLiteral("Cannot create directory for '%1'")
                             .arg(op.targetPath));
                cancelAll(saveFiles);
                return false;
            }

            auto sf = std::make_unique<QSaveFile>(op.targetPath);
            if (!sf->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                setError(lastError,
                         QStringLiteral("Cannot open QSaveFile for '%1'")
                             .arg(op.targetPath));
                cancelAll(saveFiles);
                return false;
            }

            if (!op.writer(*sf))
            {
                setError(lastError,
                         QStringLiteral("Writer callback failed for '%1'")
                             .arg(op.targetPath));
                sf->cancelWriting();
                cancelAll(saveFiles);
                return false;
            }

            sf->flush();
            if (sf->error() != QFile::NoError)
            {
                setError(lastError,
                         QStringLiteral("Error during flush for '%1' (code %2)")
                             .arg(op.targetPath)
                             .arg(sf->error()));
                sf->cancelWriting();
                cancelAll(saveFiles);
                return false;
            }

            Meta m;
            m.targetPath = op.targetPath;
            m.backupPath = op.targetPath + QStringLiteral("_backup");
            metas.push_back(std::move(m));
            saveFiles.push_back(std::move(sf));
        }

        return true;
    }

    bool renameOriginalsToBackups(
        QVector<Meta>& metas,
        std::vector<std::unique_ptr<QSaveFile>>& saveFiles, QString& lastError)
    {
        for (int i = 0; i < metas.size(); ++i)
        {
            auto& m = metas[i];

            if (QFile::exists(m.backupPath))
            {
                QFile::remove(m.backupPath); // best effort
            }
            if (QFile::exists(m.targetPath))
            {
                if (!QFile::rename(m.targetPath, m.backupPath))
                {
                    setError(lastError,
                             QStringLiteral(
                                 "Failed to rename original to backup for '%1'")
                                 .arg(m.targetPath));

                    // rollback renames already done
                    for (int j = 0; j < i; ++j)
                    {
                        auto& r = metas[j];
                        if (QFile::exists(r.backupPath) &&
                            !QFile::exists(r.targetPath))
                        {
                            QFile::rename(r.backupPath, r.targetPath);
                        }
                    }

                    cancelAll(saveFiles);
                    return false;
                }
            }
        }

        return true;
    }

    bool commitTempFiles(QVector<Meta>& metas,
                         std::vector<std::unique_ptr<QSaveFile>>& saveFiles,
                         QString& lastError)
    {
        bool ok = true;

        for (int i = 0; i < metas.size(); ++i)
        {
            auto& m = metas[i];
            auto& sf = saveFiles[i];

            if (!sf->commit())
            {
                setError(lastError,
                         QStringLiteral("QSaveFile commit failed for '%1'")
                             .arg(m.targetPath));
                ok = false;
                break;
            }
        }

        if (!ok)
        {
            // Best-effort rollback: remove new files and restore backups.
            for (int i = 0; i < metas.size(); ++i)
            {
                auto& m = metas[i];
                if (QFile::exists(m.targetPath))
                {
                    QFile::remove(m.targetPath);
                }
                if (QFile::exists(m.backupPath))
                {
                    QFile::rename(m.backupPath, m.targetPath);
                }
            }
            return false;
        }

        return true;
    }

} // unnamed namespace

// -----------------------------------------------------------------------------
// GtBatchSaver methods
// -----------------------------------------------------------------------------

void
GtBatchSaver::addOp(const QString& targetPath,
                        const std::function<bool(QIODevice&)>& writer)
{
    Op op;
    op.targetPath = targetPath;
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
    });
}

bool
GtBatchSaver::commit()
{
    m_lastError.clear();

    if (m_ops.isEmpty())
        return true;

    QVector<Meta> metas;
    std::vector<std::unique_ptr<QSaveFile>> saveFiles;

    if (!prepareSaveFiles(m_ops, metas, saveFiles, m_lastError))
        return false;

    if (!renameOriginalsToBackups(metas, saveFiles, m_lastError))
        return false;

    return commitTempFiles(metas, saveFiles, m_lastError);
}

QString
GtBatchSaver::errorString() const
{
    return m_lastError;
}
