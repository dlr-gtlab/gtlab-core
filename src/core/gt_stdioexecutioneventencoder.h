/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTSTDIOEXECUTIONEVENTENCODER_H
#define GTSTDIOEXECUTIONEVENTENCODER_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <mutex>
#include <optional>

#include <QJsonObject>
#include <QObject>

class GtObject;
class QIODevice;

/**
 * @brief Boundary adapter that writes ADR 0001 Worker Slice 1 records.
 *
 * Connect GtExecutionEventStream::eventPublished() to encodeEvent() when a
 * one-shot worker needs protocol output. Each protocol record is assembled as
 * compact JSON and written as one UTF-8 line beginning with
 * "@gtlab-operation-v1 ". The supplied QIODevice is normally stdout, but is
 * injected so callers can choose the process-boundary device and tests can use
 * a buffer.
 *
 * Events may precede exactly one terminal result or failure. Once terminal,
 * this encoder rejects all further records. A process-wide write lock prevents
 * bytes from protocol records emitted by concurrent encoders from interleaving.
 * Ordinary non-prefixed stdout remains outside this class and is not protocol.
 */
class GT_CORE_EXPORT GtStdioExecutionEventEncoder : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a V1 writer for one execution and output device.
     * @param executionId Identity required in every emitted protocol object.
     * @param output Open writable device receiving complete protocol records.
     */
    explicit GtStdioExecutionEventEncoder(GtExecutionId executionId,
                                          QIODevice& output,
                                          QObject* parent = nullptr);

    /// Returns whether a terminal result or failure record was written.
    bool isTerminal() const noexcept;

public slots:
    /**
     * @brief Encodes one intermediate event record.
     *
     * JSON payloads remain JSON; Memento/XML bytes become standard Base64.
     * Events with another execution identity or events after completion are
     * ignored so one encoder cannot violate its invocation framing.
     */
    void encodeEvent(GtExecutionEvent event);

public:
    /**
     * @brief Writes the sole successful terminal record.
     * @param result Nullable detached result; nullptr encodes as JSON null.
     * @return true only when the complete record was written.
     */
    bool encodeResult(GtObject const* result);

    /**
     * @brief Writes the sole failed terminal record.
     * @param errorCode Stable, non-empty machine-readable failure code.
     * @param message Non-empty human-readable failure description.
     * @param details Optional JSON value with structured failure details.
     * @return true only when the complete record was written.
     */
    bool encodeFailure(QString errorCode, QString message,
                       std::optional<QJsonValue> details = std::nullopt);

private:
    bool writeRecord(QJsonObject record, bool terminal);

    GtExecutionId m_executionId;
    QIODevice& m_output;
    bool m_terminal {false};
    mutable std::mutex m_mutex;
    static std::mutex s_outputMutex;
};

#endif // GTSTDIOEXECUTIONEVENTENCODER_H
