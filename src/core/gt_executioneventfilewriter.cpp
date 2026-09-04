/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executioneventfilewriter.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <utility>

GtExecutionEventFileWriter::GtExecutionEventFileWriter(QString filePath,
                                                         QObject* parent) :
    QObject(parent),
    m_file(filePath),
    m_filePath(std::move(filePath))
{
    m_file.open(QIODevice::WriteOnly | QIODevice::Append);
}

bool GtExecutionEventFileWriter::isOpen() const
{
    return m_file.isOpen();
}

QString const& GtExecutionEventFileWriter::filePath() const noexcept
{
    return m_filePath;
}

void GtExecutionEventFileWriter::writeEvent(GtExecutionEvent event)
{
    if (!m_file.isOpen())
    {
        return;
    }

    QJsonObject record {
        {QStringLiteral("executionId"), event.executionId().toString()},
        {QStringLiteral("sequence"), static_cast<qint64>(event.sequence())},
        {QStringLiteral("eventType"), event.eventType()},
        {QStringLiteral("payload"), event.payload().isUndefined()
                                    ? QJsonValue::Null
                                    : event.payload()}
    };

    QByteArray line = QJsonDocument(record).toJson(QJsonDocument::Compact);
    line += '\n';

    m_file.write(line);
    m_file.flush();
}
