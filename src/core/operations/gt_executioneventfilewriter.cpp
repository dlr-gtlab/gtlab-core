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
    m_file(std::make_unique<QFile>(filePath)), m_filePath(std::move(filePath))
{
    if (!m_file->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_hasError = true;
        m_errorString = m_file->errorString();
    }
}

GtExecutionEventFileWriter::GtExecutionEventFileWriter(
    std::unique_ptr<QFile> file, QString filePath, QObject* parent) :
    QObject(parent),
    m_file(std::move(file)), m_filePath(std::move(filePath))
{
}

bool
GtExecutionEventFileWriter::isOpen() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_file->isOpen();
}

QString const&
GtExecutionEventFileWriter::filePath() const noexcept
{
    return m_filePath;
}

bool
GtExecutionEventFileWriter::hasError() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_hasError;
}

QString
GtExecutionEventFileWriter::errorString() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_errorString;
}

void
GtExecutionEventFileWriter::writeEvent(GtExecutionEvent const& event)
{
    QString error;
    bool emitFailure = false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_hasError)
        {
            return;
        }

        QJsonObject record{
            {QStringLiteral("executionId"), event.executionId().toString()},
            {QStringLiteral("sequence"), static_cast<qint64>(event.sequence())},
            {QStringLiteral("eventType"), event.eventType()},
            {QStringLiteral("payload"), event.payload().isUndefined()
                                            ? QJsonValue::Null
                                            : event.payload()}};

        QByteArray line = QJsonDocument(record).toJson(QJsonDocument::Compact);
        line += '\n';

        const qint64 sizeBeforeWrite = m_file->size();
        const qint64 written = m_file->write(line);
        if (written != line.size())
        {
            error = written < 0 ? m_file->errorString()
                                : QStringLiteral("Short write to event file");
        }
        else if (!m_file->flush())
        {
            error = m_file->errorString();
        }

        if (!error.isEmpty() && !m_file->resize(sizeBeforeWrite))
        {
            error +=
                QStringLiteral("; failed to roll back partial event record: %1")
                    .arg(m_file->errorString());
        }

        if (!error.isEmpty())
        {
            m_hasError = true;
            m_errorString = error;
            emitFailure = true;
        }
    }

    if (emitFailure)
    {
        emit writeFailed(std::move(error));
    }
}
