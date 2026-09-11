/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executioneventfilewriter.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <mutex>
#include <utility>

struct GtExecutionEventFileWriter::Impl
{
    explicit Impl(QString filePath) :
        file(std::make_unique<QFile>(filePath)), filePath(std::move(filePath))
    {
    }

    Impl(std::unique_ptr<QFile> file, QString filePath) :
        file(std::move(file)), filePath(std::move(filePath))
    {
    }

    std::unique_ptr<QFile> file;
    QString const filePath;
    mutable std::mutex mutex;
    bool hasError{false};
    QString errorString;
};

GtExecutionEventFileWriter::GtExecutionEventFileWriter(QString filePath,
                                                       QObject* parent) :
    QObject(parent),
    m_impl(std::make_unique<Impl>(std::move(filePath)))
{
    if (!m_impl->file->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_impl->hasError = true;
        m_impl->errorString = m_impl->file->errorString();
    }
}

GtExecutionEventFileWriter::GtExecutionEventFileWriter(
    std::unique_ptr<QFile> file, QString filePath, QObject* parent) :
    QObject(parent),
    m_impl(std::make_unique<Impl>(std::move(file), std::move(filePath)))
{
}

GtExecutionEventFileWriter::~GtExecutionEventFileWriter() = default;

bool
GtExecutionEventFileWriter::isOpen() const
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    return m_impl->file->isOpen();
}

QString const&
GtExecutionEventFileWriter::filePath() const noexcept
{
    return m_impl->filePath;
}

bool
GtExecutionEventFileWriter::hasError() const
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    return m_impl->hasError;
}

QString
GtExecutionEventFileWriter::errorString() const
{
    std::lock_guard<std::mutex> lock(m_impl->mutex);
    return m_impl->errorString;
}

void
GtExecutionEventFileWriter::writeEvent(GtExecutionEvent const& event)
{
    QString error;
    bool emitFailure = false;

    {
        std::lock_guard<std::mutex> lock(m_impl->mutex);
        if (m_impl->hasError)
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

        const qint64 sizeBeforeWrite = m_impl->file->size();
        const qint64 written = m_impl->file->write(line);
        if (written != line.size())
        {
            error = written < 0 ? m_impl->file->errorString()
                                : QStringLiteral("Short write to event file");
        }
        else if (!m_impl->file->flush())
        {
            error = m_impl->file->errorString();
        }

        if (!error.isEmpty() && !m_impl->file->resize(sizeBeforeWrite))
        {
            error +=
                QStringLiteral("; failed to roll back partial event record: %1")
                    .arg(m_impl->file->errorString());
        }

        if (!error.isEmpty())
        {
            m_impl->hasError = true;
            m_impl->errorString = error;
            emitFailure = true;
        }
    }

    if (emitFailure)
    {
        emit writeFailed(std::move(error));
    }
}
