/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.09.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERRESPONSE_H
#define GTPROCESSRUNNERRESPONSE_H

#include "gt_processrunnerglobals.h"

class GT_CORE_EXPORT GtProcessRunnerResponse : public GtObject
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerResponse(QString const& sessionId = {},
                                        QString const& commandUuid = {},
                                        gt::process_runner::State state = {},
                                        gt::process_runner::Error code = {},
                                        QString const& error = {});

    bool appendPayload(GtObject* object, const QString& name = {});

    /** getter **/
    QString /*const&*/ sessionId() const;

    QString /*const&*/ commandUuid() const;

    gt::process_runner::State runnerState() const;

    gt::process_runner::Error errorCode() const;

    QString /*const&*/ error() const;

    bool success() const;

    template <typename T = GtObject*>
    T payload(QString const& objectName)
    {
        return findDirectChild<T>(objectName);
    }

private:

    GtStringProperty m_pSessionId;
    GtStringProperty m_pCommandUuid;
    GtEnumProperty<gt::process_runner::State> m_pState;
    GtEnumProperty<gt::process_runner::Error> m_pErrorCode;
    GtStringProperty m_pError;
};

#endif // GTPROCESSRUNNERRESPONSE_H
