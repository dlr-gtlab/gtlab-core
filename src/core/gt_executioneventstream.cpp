/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_executioneventstream.h"

#include <stdexcept>
#include <utility>

GtExecutionEventStream::GtExecutionEventStream(GtExecutionId executionId,
                                                 QObject* parent) :
    QObject(parent),
    m_executionId(std::move(executionId))
{
    qRegisterMetaType<GtExecutionEvent>();
}

GtExecutionId const& GtExecutionEventStream::executionId() const noexcept
{
    return m_executionId;
}

void GtExecutionEventStream::publish(QString eventType, QJsonValue payload)
{
    if (eventType.isEmpty())
    {
        throw std::invalid_argument("Execution event type must not be empty");
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pending.emplace_back(m_executionId, m_nextSequence++,
                               std::move(eventType), std::move(payload));

        if (m_dispatching)
        {
            return;
        }

        m_dispatching = true;
    }

    for (;;)
    {
        GtExecutionEvent event;

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_pending.empty())
            {
                m_dispatching = false;
                return;
            }

            event = std::move(m_pending.front());
            m_pending.pop_front();
        }

        emit eventPublished(event);
    }
}
