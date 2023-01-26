/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 21.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_remoteprocessrunnerstates.h"
#include "gt_coreprocessexecutor.h"
#include "gt_objectmementodiff.h"
#include "gt_package.h"
#include "gt_processdata.h"
#include "gt_externalizationmanager.h"
#include "gt_processrunnercommand.h"
#include "gt_remoteprocessrunner.h"
#include "gt_exceptions.h"


QPointer<GtAbstractProcessRunnerState>
gt::process_runner::makeState(State state,
                              MetaData& metaData,
                              GtRemoteProcessRunner& runner) noexcept(false)
{
    QPointer<GtRemoteProcessRunner> ptr{&runner};
    MetaData* m = &metaData;

    switch (state)
    {
    case UninitializedState:
        return new GtUninitializedProcessRunnerState{ptr, m};
    case InitializedState:
        return new GtInitializedProcessRunnerState{ptr, m};
    case RunningTaskState:
        return new GtRunningTaskProcessRunnerState{ptr, m};
    case FinishedTaskState:
        return new GtFinishedTaskProcessRunnerState{ptr, m};
    case CollectedState:
        return new GtCollectedProcessRunnerState{ptr, m};
    default:
        throw GTlabException{
            __FUNCTION__, QObject::tr("Unhandled Process Runner State!")
        };
    }
    return {};
}

GtAbstractProcessRunnerState::GtAbstractProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    QObject{runner},
    m_runner{std::move(runner)},
    m_metaData{metaData}
{
    assert(m_runner);
    assert(m_metaData);
}

std::unique_ptr<GtProcessRunnerResponse>
GtAbstractProcessRunnerState::makeResponse(Command& command,
                                           gt::process_runner::Error code,
                                           QString error)
{
    return std::make_unique<GtProcessRunnerResponse>(
        m_metaData->sessionId,
        command.uuid(),
        state(),
        code,
        std::move(error)
    );
}

std::unique_ptr<GtProcessRunnerResponse>
GtAbstractProcessRunnerState::handleCommand(Command& command)
{
    // handle ping command
    if (qobject_cast<GtProcessRunnerPingCommand*>(&command))
    {
        gtDebugId(GT_EXEC_ID).medium() << "Ping Command!";
        return makeResponse(command);
    }

    if (auto response = checkSession(command))
    {
        return response;
    }

    // check for abort command
    if (qobject_cast<GtProcessRunnerAbortCommand*>(&command))
    {
        gtDebugId(GT_EXEC_ID).medium() << "Abort Command!";
        m_runner->exit(GtRemoteProcessRunner::AbortExit);
    }

    return {};
}

std::unique_ptr<GtProcessRunnerResponse>
GtAbstractProcessRunnerState::handleInitSessionCommand(Command& command)
{
    auto* initCmd = qobject_cast<GtProcessRunnerInitSessionCommand*>(&command);

    if (!initCmd)
    {
        return {};
    }

    gtDebugId(GT_EXEC_ID).medium() << "Init Session Command!";

    // invalidate current session
    m_metaData->isSessionValid = false;

    if (!m_metaData->sessionId.isEmpty())
    {
        return makeResponse(command, gt::process_runner::InvalidStateError,
                            tr("Session was already initialized!"));
    }

    if (!initCmd->checkFootprint())
    {
        return makeResponse(command, gt::process_runner::FootprintError,
                            tr("Footprints do not match!"));
    }

    m_metaData->sessionId = command.sessionId();
    m_metaData->isSessionValid = true;

    emit stateFinished();

    return makeResponse(command);
}

std::unique_ptr<GtProcessRunnerResponse>
GtAbstractProcessRunnerState::checkSession(Command& command)
{
    // session id should not be empty
    if (command.sessionId().isEmpty())
    {
        gtWarningId(GT_EXEC_ID) << "Invalid session id";
        return makeResponse(command, gt::process_runner::InvalidSessionError,
                            tr("Invalid session id!"));
    }

    // session id must match if the process runer is in progresss
    if (stateType() != gt::process_runner::IntermediateStateType)
    {
        return {};
    }

    // check session id
    if (m_metaData->sessionId != command.sessionId())
    {
        return makeResponse(command, gt::process_runner::InvalidSessionError,
                            tr("Session id mismatch!"));
    }

    return {};
}

GtUninitializedProcessRunnerState::GtUninitializedProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    GtAbstractProcessRunnerState{runner, metaData}
{
    setObjectName("UninitializedState");
}

std::unique_ptr<GtProcessRunnerResponse>
GtUninitializedProcessRunnerState::handleCommand(Command& command)
{
    // handle default commands
    if (auto response = GtAbstractProcessRunnerState::handleCommand(command))
    {
        return response;
    }

    return handleInitSessionCommand(command);
}


GtInitializedProcessRunnerState::GtInitializedProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    GtAbstractProcessRunnerState{runner, metaData}
{
    setObjectName("InitializedState");
}

std::unique_ptr<GtProcessRunnerResponse>
GtInitializedProcessRunnerState::handleCommand(Command& command)
{
    // handle default commands
    if (auto response = GtAbstractProcessRunnerState::handleCommand(command))
    {
        return response;
    }

    // check for run task command
    auto* runCmd = qobject_cast<GtProcessRunnerRunTaskCommand*>(&command);

    if (!runCmd)
    {
        return {};
    }

    gtDebugId(GT_EXEC_ID) << "Run Task Command!";

    // sanity check
    assert(!m_metaData->task);
    assert(!m_metaData->source);

    auto& executor = gt::currentProcessExecutor();

    // shortcut for meta dasta
    auto* m = m_metaData;
    m->task = runCmd->task();
    m->source = runCmd->source();

    gtDebugId(GT_EXEC_ID) << "TASK:  " << m->task;
    gtDebugId(GT_EXEC_ID) << "SOURCE:" << m->source;

    if (!m->task || !m->source)
    {
        return makeResponse(command, gt::process_runner::InvalidArgumentError,
                            tr("Invalid task or source provided!"));
    }

    // setup process data
    auto* processData = new GtProcessData;
    processData->setParent(m_runner);

    // take ownership
    processData->appendChild(m->task);
    m->source->setParent(m_runner);

    // project path
    QString projectPath = runCmd->projectPath();

    gtDebugId(GT_EXEC_ID) << tr("Using project path: %1").arg(projectPath);

    gtExternalizationManager->setProjectDir(projectPath);

    if (!executor.setCustomProjectPath(projectPath))
    {
        return makeResponse(command, gt::process_runner::RunTaskError,
                            tr("Failed to set project path!"));
    }

    // connect task state changes
    auto pcs = m->task->findChildren<GtProcessComponent*>();
    pcs << m->task;

    for (auto* pc : qAsConst(pcs))
    {
        auto success =
            connect(pc, &GtProcessComponent::stateChanged,
                    m_runner, &GtRemoteProcessRunner::onTaskStateChanged,
                    Qt::QueuedConnection);
        assert(success);
    }

    // connect task finished signal
    auto success =
        connect(&executor, &GtCoreProcessExecutor::allTasksCompleted,
                m_runner, &GtRemoteProcessRunner::onTaskFinished,
                Qt::QueuedConnection);
    assert(success);

    // create before mementos
    m->initialTaskMemento = m->task->toMemento();

    auto packages = m->source->findDirectChildren<GtPackage*>();

    std::transform(std::cbegin(packages), std::cend(packages),
                   std::back_inserter(m->initialDataMementos),
                   [](GtPackage* package){
        return package->toMemento();
    });

    if (!executor.setSource(m->source))
    {
        return makeResponse(command, gt::process_runner::RunTaskError,
                            tr("Failed to set source!"));
    }

    if (!executor.runTask(m->task))
    {
        return makeResponse(command, gt::process_runner::RunTaskError,
                           tr("Triggering task execution failed!"));
    }

    emit stateFinished();

    // dont delete command
    return makeResponse(command);
}


GtRunningTaskProcessRunnerState::GtRunningTaskProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    GtAbstractProcessRunnerState{runner, metaData}
{
    setObjectName("RunningTaskState");
}

std::unique_ptr<GtProcessRunnerResponse>
GtRunningTaskProcessRunnerState::handleCommand(Command& command)
{
    // handle default commands
    return GtAbstractProcessRunnerState::handleCommand(command);
}

GtFinishedTaskProcessRunnerState::GtFinishedTaskProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    GtAbstractProcessRunnerState{runner, metaData}
{
    setObjectName("FinishedTaskState");
}

std::unique_ptr<GtProcessRunnerResponse>
GtFinishedTaskProcessRunnerState::handleCommand(Command& command)
{
    // handle default commands
    if (auto response = GtAbstractProcessRunnerState::handleCommand(command))
    {
        return response;
    }

    // check for collect task command
    auto* collectCmd = qobject_cast<GtProcessRunnerCollectTaskCommand*>(&command);

    if (!collectCmd)
    {
        return {};
    }

    gtDebugId(GT_EXEC_ID) << "Collect Task Command!";

    if (!m_metaData->task || !m_metaData->source)
    {
        return makeResponse(command, gt::process_runner::RunTaskError,
                            tr("Invalid task or source!"));
    }

    QString taskUuid = collectCmd->taskUuid();

    if (m_metaData->task->uuid() != taskUuid)
    {
        return makeResponse(command, gt::process_runner::InvalidArgumentError,
                            tr("Failed to find Task by Uuid!"));
    }

    // create after mementos
    auto packages = m_metaData->source->findDirectChildren<GtPackage*>();
    auto const& mementos = m_metaData->initialDataMementos;
    int size = mementos.size();

    // check if all uuids match, order of objects must match!
    bool canMergeData =
            (size == packages.size()) &&
            std::equal(std::cbegin(packages), std::cend(packages),
                       std::cbegin(mementos), [](auto* p, auto const& m){
        return p && !m.isNull() && p->uuid() == m.uuid();
    });

    if (!canMergeData)
    {
        // for debugging
        for (const auto& p : qAsConst(packages))
        {
            gtErrorId(GT_EXEC_ID) << "Package"
                             << p->staticMetaObject.className()
                             << p->objectName()
                             << p->uuid() ;
        }
        for (const auto& m : qAsConst(mementos))
        {
            gtErrorId(GT_EXEC_ID) << "Memento"
                             << m.className()
                             << m.ident()
                             << m.uuid() ;
        }

        return makeResponse(command, gt::process_runner::CollectTaskError,
                            tr("Failed to create data diff!"));
    }

    // diff of data objects
    GtObjectMementoDiff dataDiff;
    for (int i = 0; i < size; ++i)
    {
        GtObjectMemento const& mOld = mementos[i];
        GtObjectMemento mNew = packages[i]->toMemento(true);
        GtObjectMementoDiff diff{mOld, mNew};

        dataDiff << diff;
    }

    // diff of task (for property changes
    GtObjectMementoDiff taskDiff;
    GtObjectMemento mOld = m_metaData->initialTaskMemento;
    GtObjectMemento mNew = m_metaData->task->toMemento();
    taskDiff << GtObjectMementoDiff{mOld, mNew};

    auto response = makeResponse(command);

    auto* dataPayload = new gt::process_runner::MementoDiffPayload{dataDiff};
    response->appendPayload(dataPayload, m_metaData->source->objectName());
    auto* taskPayload = new gt::process_runner::MementoDiffPayload{taskDiff};
    response->appendPayload(taskPayload, m_metaData->task->objectName());

    emit stateFinished();

    return response;
}

GtCollectedProcessRunnerState::GtCollectedProcessRunnerState(
        QPointer<GtRemoteProcessRunner> runner,
        gt::process_runner::MetaData* metaData) :
    GtAbstractProcessRunnerState{runner, metaData}
{
    setObjectName("CollectedState");
}

std::unique_ptr<GtProcessRunnerResponse>
GtCollectedProcessRunnerState::handleCommand(Command& command)
{
    // handle default commands
    if (auto response = GtAbstractProcessRunnerState::handleCommand(command))
    {
        return response;
    }

    return handleInitSessionCommand(command);
}
