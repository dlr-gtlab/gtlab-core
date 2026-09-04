/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTFILEWRITER_H
#define GTEXECUTIONEVENTFILEWRITER_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <mutex>

#include <QFile>
#include <QObject>

/**
 * @brief File-backed observer that writes an NDJSON event stream.
 *
 * Connect writeEvent() to GtExecutionEventStream::eventPublished() for a
 * worker-owned events.ndjson file. Each accepted event becomes exactly one
 * compact JSON object followed by a newline. The file is intentionally
 * exclusive to event records; ordinary stdout, stderr, and log output must be
 * written elsewhere.
 *
 * The writer is a local Qt observer. It does not open or manage a stream for
 * remote transport. Callers can inspect hasError() and errorString() to detect
 * a failed open, write, or flush, and may react to writeFailed().
 */
class GT_CORE_EXPORT GtExecutionEventFileWriter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Opens @p filePath for appending event records.
     * @param filePath Path of the worker-owned event stream file.
     * @param parent Optional Qt owner of the writer.
     */
    explicit GtExecutionEventFileWriter(QString filePath,
                                        QObject* parent = nullptr);

    /// Returns whether the event file was opened successfully.
    bool isOpen() const;
    /// Returns the event stream file path.
    QString const& filePath() const noexcept;
    /// Returns whether opening, writing, or flushing the event file failed.
    bool hasError() const;
    /// Returns the first event-file I/O error, or an empty string on success.
    QString errorString() const;

public slots:
    /// Appends one compact JSON event record and its terminating newline.
    void writeEvent(GtExecutionEvent event);

signals:
    /// Emitted once when an event record can no longer be persisted.
    void writeFailed(QString error);

private:
    QFile m_file;
    QString m_filePath;
    mutable std::mutex m_mutex;
    bool m_hasError{false};
    QString m_errorString;
};

#endif // GTEXECUTIONEVENTFILEWRITER_H
