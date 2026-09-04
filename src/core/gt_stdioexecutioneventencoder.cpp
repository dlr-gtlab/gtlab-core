/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_stdioexecutioneventencoder.h"

#include "gt_object.h"
#include "gt_objectmemento.h"

#include <QIODevice>
#include <QJsonDocument>

#include <utility>

std::mutex GtStdioExecutionEventEncoder::s_outputMutex;

namespace
{
constexpr char prefix[] = "@gtlab-operation-v1 ";

QJsonObject commonRecord(QString const& kind, GtExecutionId const& executionId)
{
    return {{QStringLiteral("version"), 1},
            {QStringLiteral("kind"), kind},
            {QStringLiteral("executionId"), executionId.toString()}};
}
}

GtStdioExecutionEventEncoder::GtStdioExecutionEventEncoder(
    GtExecutionId executionId, QIODevice& output, QObject* parent) :
    QObject(parent),
    m_executionId(std::move(executionId)),
    m_output(output)
{
}

bool GtStdioExecutionEventEncoder::isTerminal() const noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_terminal;
}

void GtStdioExecutionEventEncoder::encodeEvent(GtExecutionEvent event)
{
    QJsonObject record = commonRecord(QStringLiteral("event"), event.executionId());
    record.insert(QStringLiteral("sequence"), static_cast<qint64>(event.sequence()));
    record.insert(QStringLiteral("eventType"), event.eventType());

    if (event.payloadEncoding() == GtExecutionEvent::PayloadEncoding::Json) {
        record.insert(QStringLiteral("payloadEncoding"), QStringLiteral("json"));
        record.insert(QStringLiteral("payload"),
                      event.jsonPayload().isUndefined() ? QJsonValue::Null :
                                                          event.jsonPayload());
    } else {
        record.insert(QStringLiteral("payloadEncoding"),
                      QStringLiteral("memento-xml-base64"));
        record.insert(QStringLiteral("payload"),
                      QString::fromLatin1(event.mementoXml().toBase64()));
    }

    writeRecord(std::move(record), false);
}

bool GtStdioExecutionEventEncoder::encodeResult(GtObject const* result)
{
    QJsonObject record = commonRecord(QStringLiteral("result"), m_executionId);

    if (!result) {
        record.insert(QStringLiteral("resultEncoding"), QStringLiteral("null"));
        record.insert(QStringLiteral("result"), QJsonValue::Null);
    } else {
        record.insert(QStringLiteral("resultEncoding"),
                      QStringLiteral("memento-xml-base64"));
        record.insert(QStringLiteral("result"), QString::fromLatin1(
                          result->toMemento().toByteArray().toBase64()));
    }

    return writeRecord(std::move(record), true);
}

bool GtStdioExecutionEventEncoder::encodeFailure(
    QString errorCode, QString message, std::optional<QJsonValue> details)
{
    if (errorCode.isEmpty() || message.isEmpty()) {
        return false;
    }

    QJsonObject record = commonRecord(QStringLiteral("failure"), m_executionId);
    record.insert(QStringLiteral("errorCode"), std::move(errorCode));
    record.insert(QStringLiteral("message"), std::move(message));
    if (details) {
        record.insert(QStringLiteral("details"), *details);
    }

    return writeRecord(std::move(record), true);
}

bool GtStdioExecutionEventEncoder::writeRecord(QJsonObject record, bool terminal)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_terminal ||
        (!terminal && record.value(QStringLiteral("executionId")) !=
                          m_executionId.toString())) {
        return false;
    }

    QByteArray line(prefix);
    line += QJsonDocument(record).toJson(QJsonDocument::Compact);
    line += '\n';

    std::lock_guard<std::mutex> outputLock(s_outputMutex);
    if (m_output.write(line) != line.size()) {
        return false;
    }

    if (terminal) {
        m_terminal = true;
    }

    return true;
}
