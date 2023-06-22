/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnernotification.h"

GtProcessRunnerNotification::GtProcessRunnerNotification(
        QString const& sessionId,
        NotificationType type,
        bool optional) :
    m_pSessionId("sessionId", tr("Session ID"), tr("Session ID"), sessionId),
    m_pNotificationType("notificationType", tr("Notification Type"),
                        tr("Notification Type"), type),
    m_pOptional("optional", tr("Optional"), tr("Optional"), optional)
{
    setObjectName("GtProcessNotification");

    registerProperty(m_pSessionId);

    registerProperty(m_pNotificationType);

    registerProperty(m_pOptional);
}

bool
GtProcessRunnerNotification::appendPayload(GtObject* object,
                                           const QString& name)
{
    if (!object)
    {
        return false;
    }

    if (!name.isEmpty())
    {
        object->setObjectName(name);
    }

    object->setParent(this);

    return true;
}

QString /*const&*/
GtProcessRunnerNotification::sessionId() const
{
    return m_pSessionId;
}

GtProcessRunnerNotification::NotificationType
GtProcessRunnerNotification::notificationType() const
{
    return m_pNotificationType;
}
