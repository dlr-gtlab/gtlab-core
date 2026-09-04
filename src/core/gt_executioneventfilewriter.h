/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTFILEWRITER_H
#define GTEXECUTIONEVENTFILEWRITER_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

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
 * remote transport, and callers can use isOpen() to detect an unavailable file.
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

public slots:
    /// Appends one compact JSON event record and its terminating newline.
    void writeEvent(GtExecutionEvent event);

private:
    QFile m_file;
    QString m_filePath;
};

#endif // GTEXECUTIONEVENTFILEWRITER_H
