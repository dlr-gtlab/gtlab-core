/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 24.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnerglobals.h"
#include "gt_coreapplication.h"
#include "gt_objectmementodiff.h"
#include "gt_processrunnerresponse.h"

#include <QDir>

const quint16 gt::process_runner::S_DEFAULT_PORT = 8040;
const QString gt::process_runner::S_DEFAULT_HOST{"localhost"};
const QString gt::process_runner::S_ACCESS_ID{"Process Runner"};

QString
gt::process_runner::portLockFilePath(quint16 port)
{
    assert(gtApp);
    assert(!gtApp->roamingPath().isEmpty());

    QDir appDataDir = gtApp->roamingPath();

    return appDataDir.absoluteFilePath(QStringLiteral("port_%1.lock")
                                       .arg(port));
}

QString
gt::process_runner::errorMessage(GtProcessRunnerResponse* response)
{
    return QStringLiteral("Error: ") +
           (response ? response->error() : QObject::tr("Invalid Response"));
}

gt::process_runner::ProcessComponentStatePayload::ProcessComponentStatePayload(
        GtProcessComponent* pc) :
    m_pComponent{"component", "Process Component", "Process Component"},
    m_pComponentState{"state", "State", "State of Process Component"}
{
    setObjectName("ProcessComponentMetaData");

    registerProperty(m_pComponent);
    registerProperty(m_pComponentState);

    if (pc)
    {
        m_pComponentState = pc->currentState();
        m_pComponent.setVal(pc->uuid());
    }
}

GtProcessComponent::STATE
gt::process_runner::ProcessComponentStatePayload::state() const
{
    return m_pComponentState;
}

QString /*const&*/
gt::process_runner::ProcessComponentStatePayload::componentUuid() const
{
    return m_pComponent;
}

gt::process_runner::MementoDiffPayload::MementoDiffPayload() :
    MementoDiffPayload{{}}
{

}

gt::process_runner::MementoDiffPayload::MementoDiffPayload(
        const GtObjectMementoDiff& diff) :
    m_pMementoDiff("mementoDiff", tr("Memento Diff"), tr("Memento Diff"),
                   diff.toByteArray())
{
    setObjectName("MementoDiff");

    registerProperty(m_pMementoDiff);
}

GtObjectMementoDiff
gt::process_runner::MementoDiffPayload::mementoDiff() const
{
    return GtObjectMementoDiff{m_pMementoDiff.getVal().toLatin1()};
}
