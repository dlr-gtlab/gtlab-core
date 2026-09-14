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
#include <ostream>

#include <QJsonObject>
#include <QObject>

class GtObject;

/**
 * @brief Writes V1 compatibility records to a standard stream.
 *
 * Connect writeEvent() to GtExecutionEventStream::eventPublished() when V1
 * protocol output is needed. Each record is one compact JSON line with the
 * "@gtlab-operation-v1 " prefix.
 *
 * This is a fallback adapter. Use the event file as the primary worker channel.
 * The encoder serializes its own writes, but cannot control other stdout writes.
 */
class GT_CORE_EXPORT GtStdioExecutionEventEncoder : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a V1 encoder for one execution.
     *
     * @param executionId ID written to every protocol record.
     * @param output Stream that receives protocol records.
     * @param parent Optional Qt owner.
     *
     * The encoder does not own @p output. Keep it alive while the encoder is used.
     */
    explicit GtStdioExecutionEventEncoder(GtExecutionId executionId,
                                          std::ostream& output,
                                          QObject* parent = nullptr);

    /**
     * @brief Returns whether a terminal record was written.
     */
    bool isTerminal() const noexcept;

public slots:
    /**
     * @brief Writes an intermediate event record.
     *
     * Events for another execution, and events after completion, are ignored.
     */
    void writeEvent(GtExecutionEvent const& event);

public:
    /**
     * @brief Writes the successful terminal record.
     *
     * @param result Detached result. A null pointer is encoded as JSON null.
     *
     * @return True if the complete record was written.
     */
    bool encodeResult(GtObject const* result);

    /**
     * @brief Writes the failed terminal record.
     *
     * @param errorCode Non-empty machine-readable failure code.
     * @param message Non-empty description of the failure.
     * @param details Optional JSON details.
     *
     * @return True if the complete record was written.
     */
    bool encodeFailure(QString errorCode, QString message,
                       std::optional<QJsonValue> details = std::nullopt);

private:
    bool writeRecord(QJsonObject record, bool terminal);

    GtExecutionId m_executionId;
    std::ostream& m_output;
    bool m_terminal{false};
    bool m_outputFailed{false};
    mutable std::mutex m_mutex;
    static std::mutex s_outputMutex;
};

#endif // GTSTDIOEXECUTIONEVENTENCODER_H
