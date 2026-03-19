/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTREMOTEPROCESSRUNNERSTATES_H
#define GTREMOTEPROCESSRUNNERSTATES_H

#include "gt_processrunnerresponse.h"
#include "gt_task.h"
#include <QObject>

class GtProcessRunnerCommand;
class GtRemoteProcessRunner;
class GtAbstractProcessRunnerState;

namespace gt
{

namespace process_runner
{

/**
 * @brief The MetaData struct
 * Meta data for the state handlers
 */
struct MetaData
{
    /// Session id of the client. Used to verify that the correct
    /// client is connected
    QString sessionId{};

    /// Root object of all data objects, that the tasks runs on
    QPointer<GtObject> source;

    /// Task that is beeing executed
    QPointer<GtTask> task;

    /// Initial mementos of the child objects of source. Used for generating
    /// the diff after task execution
    QVector<GtObjectMemento> initialDataMementos;

    GtObjectMemento initialTaskMemento;

    bool isSessionValid{false};
};

/**
 * @brief The Step enum.
 * Indicates the type of a state
 */
enum StateType
{
    StartStateType,
    IntermediateStateType,
    FinishStateType
};

/**
 * @brief Creates and returns a pointer to the new state handler. State is owned
 * by the runner.
 * @param state Next state
 * @param metaData Meta data for the state
 * @param runner Process runner
 * @return Pointer to state
 */
QPointer<GtAbstractProcessRunnerState> makeState(State state,
                                                 MetaData& metaData,
                                                 GtRemoteProcessRunner& runner
                                                 ) noexcept(false);


} // namespace process_runner

} // namespace gt

/**
 * @brief The GtAbstractProcessRunnerState class
 */
class GtAbstractProcessRunnerState : public QObject
{
    Q_OBJECT

public:

    using Command = GtProcessRunnerCommand;
    using Response = GtProcessRunnerResponse;

    /**
     * @brief Indicates the state type
     * @return state type
     */
    virtual gt::process_runner::StateType stateType() const
    {
        return gt::process_runner::IntermediateStateType;
    }

    /**
     * @brief Inidcates the current state
     * @return current state
     */
    virtual gt::process_runner::State state() const = 0;

    /**
     * @brief Inidcates the next state
     * @return next state
     */
    virtual gt::process_runner::State nextState() const = 0;

    /**
     * @brief Implements the logic to handle the command given
     * @param command Command to handle
     * @return Response if handled else nullptr
     */
    virtual std::unique_ptr<Response> handleCommand(Command& command);

    /**
     * @brief Helper method for creating a response
     * @param command Command to respond to
     * @param code Error code if any
     * @param error Error string if any
     * @return Reponse
     */
    std::unique_ptr<Response> makeResponse(const Command& command,
                                           gt::process_runner::Error code = {},
                                           QString error = {});

signals:

    /**
     * @brief May be emitted once the state has finished
     */
    void stateFinished();

protected:

    /**
     * @brief constructor
     * @param runner Pointer to runner (may not be null)
     * @param metaData Pointer to meta data (may not be null)
     */
    GtAbstractProcessRunnerState(QPointer<GtRemoteProcessRunner> runner,
                                 gt::process_runner::MetaData* metaData);

    /// pointer to runner instance
    QPointer<GtRemoteProcessRunner> m_runner{};

    /// pointer to meta data
    gt::process_runner::MetaData* m_metaData{};

    std::unique_ptr<Response> handleInitSessionCommand(Command& command);

private:

    /**
     * @brief Heper method to check session id of command
     * @param command COmmand to check
     * @return Response if error else nullptr
     */
    std::unique_ptr<Response> checkSession(const Command& command);
};

/**
 * @brief The GtUninitializedProcessRunnerState class
 */
class GtUninitializedProcessRunnerState : public GtAbstractProcessRunnerState
{
    Q_OBJECT

public:

    explicit GtUninitializedProcessRunnerState(
                                 QPointer<GtRemoteProcessRunner> runner,
                                 gt::process_runner::MetaData* metaData = {});

    gt::process_runner::StateType stateType() const override
    {
        return gt::process_runner::StartStateType;
    }

    gt::process_runner::State state() const override
    {
        return gt::process_runner::State::UninitializedState;
    }

    gt::process_runner::State nextState() const override
    {
        return gt::process_runner::InitializedState;
    }

    std::unique_ptr<Response> handleCommand(Command& command) override;
};

/**
 * @brief The GtInitializedProcessRunnerState class
 */
class GtInitializedProcessRunnerState : public GtAbstractProcessRunnerState
{
    Q_OBJECT

public:

    GtInitializedProcessRunnerState(QPointer<GtRemoteProcessRunner> runner,
                                    gt::process_runner::MetaData* metaData);

    gt::process_runner::State state() const override
    {
        return gt::process_runner::State::InitializedState;
    }

    gt::process_runner::State nextState() const override
    {
        return gt::process_runner::RunningTaskState;
    }

    std::unique_ptr<Response> handleCommand(Command& command) override;
};

/**
 * @brief The GtRunningTaskProcessRunnerState class
 */
class GtRunningTaskProcessRunnerState : public GtAbstractProcessRunnerState
{
    Q_OBJECT

public:

    GtRunningTaskProcessRunnerState(QPointer<GtRemoteProcessRunner> runner,
                                    gt::process_runner::MetaData* metaData);

    gt::process_runner::State state() const override
    {
        return gt::process_runner::RunningTaskState;
    }

    gt::process_runner::State nextState() const override
    {
        return gt::process_runner::FinishedTaskState;
    }

    std::unique_ptr<Response> handleCommand(Command& command) override;
};

/**
 * @brief The GtFinishedTaskProcessRunnerState class
 */
class GtFinishedTaskProcessRunnerState : public GtAbstractProcessRunnerState
{
    Q_OBJECT

public:

    GtFinishedTaskProcessRunnerState(QPointer<GtRemoteProcessRunner> runner,
                                     gt::process_runner::MetaData* metaData);

    gt::process_runner::State state() const override
    {
        return gt::process_runner::FinishedTaskState;
    }

    gt::process_runner::State nextState() const override
    {
        return gt::process_runner::CollectedState;
    }

    std::unique_ptr<Response> handleCommand(Command& command) override;
};

/**
 * @brief The GtCollectedProcessRunnerState class
 */
class GtCollectedProcessRunnerState : public GtAbstractProcessRunnerState
{
    Q_OBJECT

public:

    GtCollectedProcessRunnerState(QPointer<GtRemoteProcessRunner> runner,
                                  gt::process_runner::MetaData* metaData);

    gt::process_runner::StateType stateType() const override
    {
        return gt::process_runner::FinishStateType;
    }

    gt::process_runner::State state() const override
    {
        return gt::process_runner::CollectedState;
    }

    gt::process_runner::State nextState() const override
    {
        return gt::process_runner::InitializedState;
    }

    std::unique_ptr<Response> handleCommand(Command& command) override;
};

#endif // GTREMOTEPROCESSRUNNERSTATES_H
