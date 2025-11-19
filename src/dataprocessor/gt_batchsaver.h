/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#ifndef GT_BATCHSAVER_H
#define GT_BATCHSAVER_H

#include <gt_datamodel_exports.h>

#include <QSaveFile>
#include <QVector>
#include <QDomDocument>
#include <QByteArray>
#include <QIODevice>
#include <QString>

#include <functional>

/**
 * @brief Helper class to perform atomic, all-or-nothing file saves with backups.
 *
 * GtBatchSaver lets you register multiple file write operations (XML or arbitrary
 * data) and then commit them as a single "transaction":
 *
 * - Each target file is first written to a temporary file via QSaveFile.
 * - If all writes and flushes succeed:
 *     - any existing target files are renamed to "<file>_backup",
 *     - all temporary files are atomically committed to their final paths.
 * - If any step fails:
 *     - no new file contents are made visible,
 *     - existing files remain as they were (best-effort rollback).
 *
 * Typical usage:
 * @code
 * GtBatchSaver batch;
 *
 * // Add external object XML files
 * batch.addXml("A.xml", docA);
 * batch.addXml("B.xml", docB);
 *
 * // Add master XML last
 * batch.addXml("master.xml", masterDoc);
 *
 * if (!batch.commit())
 * {
 *     // Handle error: no file has been partially overwritten
 * }
 * @endcode
 *
 * This is useful when saving a project that consists of multiple related files
 * (master + externals): after commit(), either all files match the new state,
 * or none of them were changed.
 */
class GT_DATAMODEL_EXPORT GtBatchSaver
{
public:
    /**
     * @brief One logical file write operation.
     *
     * The writer callback will be called with a QIODevice that is backed by
     * a QSaveFile for @ref targetPath. It is expected to write the full
     * contents of the file and return true on success, false on error.
     */
    struct Op
    {
        /// Final file path to write to (after commit).
        QString targetPath;

        /// Callback that writes the file contents into the given device.
        std::function<bool(QIODevice&)> writer;
    };

    /**
     * @brief Add a generic write operation to the batch.
     *
     * @param targetPath Path of the file that should be written on commit().
     * @param writer     Callback that writes the content to the provided QIODevice.
     *
     * The @p writer is invoked during commit(), using a QSaveFile opened for
     * @p targetPath. If @p writer returns false or fails to write correctly,
     * the whole commit() will fail and no files will be updated.
     */
    void addOp(const QString& targetPath,
               const std::function<bool(QIODevice&)>& writer);

    /**
     * @brief Convenience method to add a QDomDocument as XML to the batch.
     *
     * @param targetPath  Target file path for the XML.
     * @param doc         QDomDocument to be serialized.
     * @param attrOrdered Whether to use ordered attributes or not
     *                    (if not, attribute order can be random,
     *                    but is more efficient)
     *
     * The document is serialized as UTF-8 text during commit(). On failure,
     * commit() returns false and the target file is not changed.
     */
    void addXml(const QString& targetPath, const QDomDocument& doc,
                bool attrOrdered);

    /**
     * @brief Convenience method to add raw binary data to the batch.
     *
     * @param targetPath Target file path for the data.
     * @param data       Byte array containing the full file content.
     *
     * The entire @p data buffer is written to the QSaveFile during commit().
     * If the number of bytes written does not match @p data.size(), commit()
     * will fail and no files are updated.
     */
    void addBinary(const QString& targetPath, const QByteArray& data);

    /**
     * @brief Perform the all-or-nothing save operation for all registered files.
     *
     * Workflow:
     *  - For each registered Op:
     *      - open a QSaveFile for the target,
     *      - invoke its writer() to write the data,
     *      - flush and check for I/O errors.
     *  - If any write/flush fails, all temporary files are cancelled and
     *    the function returns false; no target file is modified.
     *  - If all writes succeed:
     *      - existing targets are renamed to "<targetPath>_backup",
     *      - all QSaveFile instances are committed (atomic replace).
     *
     * @return true if all files were written and swapped successfully,
     *         false if any error occurred (in which case the original
     *         files are left unchanged as far as possible).
     */
    bool commit();

    /**
     * @brief Returns a human-readable description of the last error, if any.
     */
    QString errorString() const;

private:
    /// List of pending operations to be executed during commit().
    std::vector<Op> m_ops;
    QString m_lastError;
};

#endif // GT_BATCHSAVER_H
