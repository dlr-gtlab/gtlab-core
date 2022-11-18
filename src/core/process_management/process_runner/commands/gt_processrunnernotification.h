/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 12.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERNOTIFICATION_H
#define GTPROCESSRUNNERNOTIFICATION_H

#include "gt_processrunnerglobals.h"

class GT_CORE_EXPORT GtProcessRunnerNotification : public GtObject
{
    Q_OBJECT

    using NotificationType = gt::process_runner::NotificationType;

public:

    Q_INVOKABLE GtProcessRunnerNotification(
            QString const& sessionId = {},
            gt::process_runner::NotificationType type = {},
            bool optional = false);

    bool appendPayload(GtObject* object, const QString& name = {});

    QString /*const&*/ sessionId() const;

    NotificationType notificationType() const;

    template <typename T = GtObject*>
    T payload(QString const& objectName = {}) const
    {
        return findDirectChild<T>(objectName);
    }

    bool isOptional() const
    {
        return m_pOptional.getVal();
    }

private:

    GtStringProperty m_pSessionId;

    GtEnumProperty<NotificationType> m_pNotificationType;

    GtBoolProperty m_pOptional;
};

#endif // GTPROCESSRUNNERNOTIFICATION_H
