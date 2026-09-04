/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTEXECUTIONEVENTSTREAM_H
#define GTEXECUTIONEVENTSTREAM_H

#include "gt_core_exports.h"
#include "gt_executionevent.h"

#include <mutex>

#include <QObject>

class GtObject;

class GT_CORE_EXPORT GtExecutionEventStream : public QObject,
                                              public GtExecutionEventSink
{
    Q_OBJECT

public:
    explicit GtExecutionEventStream(GtExecutionId executionId,
                                    QObject* parent = nullptr);

    GtExecutionId const& executionId() const noexcept;
    void publish(QString eventType, QJsonValue payload = {}) override;
    void publish(QString eventType, GtObject const& payload) override;

signals:
    void eventPublished(GtExecutionEvent event);

private:
    void publish(GtExecutionEvent event);

    GtExecutionId m_executionId;
    quint64 m_nextSequence {0};
    std::recursive_mutex m_mutex;
};

#endif // GTEXECUTIONEVENTSTREAM_H
