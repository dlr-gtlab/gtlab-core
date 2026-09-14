/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTFILEWRITER_H
#define GTEXECUTIONEVENTFILEWRITER_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <memory>

#include <QFile>
#include <QObject>

/**
 * @brief Writes execution events to an NDJSON file.
 *
 * Connect writeEvent() to GtExecutionEventStream::eventPublished(). Each event
 * becomes one compact JSON line. The file is reserved for event records; write
 * normal output and logs elsewhere.
 *
 * Check hasError() and errorString() after an I/O error, or connect to
 * writeFailed().
 */
class GT_CORE_EXPORT GtExecutionEventFileWriter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Opens @p filePath to append event records.
     *
     * @param filePath Path of the event file.
     * @param parent Optional Qt owner.
     */
    explicit GtExecutionEventFileWriter(QString filePath,
                                        QObject* parent = nullptr);

    ~GtExecutionEventFileWriter() override;

    /**
     * @brief Returns whether the event file is open.
     */
    bool isOpen() const;
    /**
     * @brief Returns the event stream file path.
     */
    QString const& filePath() const noexcept;
    /**
     * @brief Returns whether event-file I/O failed.
     */
    bool hasError() const;
    /**
     * @brief Returns the first event-file I/O error.
     */
    QString errorString() const;

public slots:
    /**
     * @brief Writes one event as a compact JSON line.
     */
    void writeEvent(GtExecutionEvent const& event);

signals:
    /**
     * @brief Emitted once after an event-file I/O error.
     */
    void writeFailed(QString error);

protected:
    /**
     * @brief Creates a writer for an already open event file.
     *
     * The writer takes ownership of @p file. This constructor is for subclasses
     * that provide a custom file implementation.
     */
    GtExecutionEventFileWriter(std::unique_ptr<QFile> file, QString filePath,
                               QObject* parent = nullptr);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // GTEXECUTIONEVENTFILEWRITER_H
