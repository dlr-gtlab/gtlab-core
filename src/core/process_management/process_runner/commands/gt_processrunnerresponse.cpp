#include "gt_processrunnerresponse.h"

#include "gt_task.h"
#include "gt_objectgroup.h"

GtProcessRunnerResponse::GtProcessRunnerResponse(QString const& sessionId,
                                                 QString const& commandUuid,
                                                 gt::process_runner::State state,
                                                 gt::process_runner::Error code,
                                                 QString const& error) :
    m_pSessionId("sessionId", "Session ID", "Session ID",
                 sessionId),
    m_pCommandUuid("commandUuid", "Command Uuid", "Command Uuid to respond to",
                   commandUuid),
    m_pState("runnerState", "Runner State", "Runner State", state),
    m_pErrorCode("errorCode", "Error Code", "Error Code", code),
    m_pError("error", "Error Message", "Error Message", error)
{
    setObjectName("GtProcessResponse");

    registerProperty(m_pSessionId);
    registerProperty(m_pCommandUuid);
    registerProperty(m_pState);
    registerProperty(m_pErrorCode);
    registerProperty(m_pError);
}

bool
GtProcessRunnerResponse::appendPayload(GtObject* object, const QString& name)
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
GtProcessRunnerResponse::sessionId() const
{
    return m_pSessionId;
}

QString /*const&*/
GtProcessRunnerResponse::commandUuid() const
{
    return m_pCommandUuid.getVal();
}

gt::process_runner::State
GtProcessRunnerResponse::runnerState() const
{
    return m_pState.getVal();
}

gt::process_runner::Error
GtProcessRunnerResponse::errorCode() const
{
    return m_pErrorCode.getVal();
}

QString /*const&*/
GtProcessRunnerResponse::error() const
{
    return m_pError;
}

bool
GtProcessRunnerResponse::success() const
{
    return m_pErrorCode == gt::process_runner::NoError;
}
