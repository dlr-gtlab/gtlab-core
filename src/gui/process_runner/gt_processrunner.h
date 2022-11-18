/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 2.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNER_H
#define GTPROCESSRUNNER_H

#include <QObject>
#include <QPointer>
#include <QThread>

#include "gt_coreprocessexecutor.h"
#include "gt_processrunnertransceiver.h"
#include "gt_gui_exports.h"

class GtTask;
class GtProcessComponent;

namespace gt { namespace process_runner { class ProcessComponentStatePayload; }}

/**
 * @brief The GtProcessRunner class
 * Interface for running a task using the remote process runner
 */
class GT_GUI_EXPORT GtProcessRunner : public GtCoreProcessExecutor
{
    Q_OBJECT

    using ConnectionState = GtProcessRunnerTransceiver::ConnectionState;
    using ProcessComponentStatePayload = gt::process_runner::ProcessComponentStatePayload;

public:

    /// Id of this executor
    static const std::string S_ID;

    /**
     * @brief Destructor
     */
    ~GtProcessRunner();

    /**
     * @brief Instance
     * @return Instance
     */
    static GtProcessRunner& instance();

    /**
     * @brief Returns the connection state of the transceiver
     * @return Connection state
     */
    ConnectionState connectionState() const;

signals:

    /**
     * @brief EMitted once the connection state changed
     */
    void connectionStateChanged();

protected:

    /**
     * @brief Terminates the current task by aborting the remote process
     * runner execution. Blocking call.
     * @return success
     */
    bool terminateCurrentTask() override;

    /**
     * @brief Executes the current task by connection to the remote process
     * runner and transmitting a run command. Non-blocking.
     */
    void execute() override;

private:

    /// private impl
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief Constructor
     */
    GtProcessRunner();

    /**
     * @brief Triggers the task collection
     */
    void onTaskFinishedSuccessfully();

    /**
     * @brief Triggers the task collection
     */
    void onTaskFinishedUnsuccessfully();

    /**
     * @brief Updates the state of the affected process component
     * @param meta Meta data describing the process component and the state
     */
    void onTaskStateChange(ProcessComponentStatePayload* payload);

    /**
     * @brief Updates the monitoring property of the affected process component
     * @param other Updated process component to apply
     */
    void onTaskPropertyChange(GtProcessComponent* other);

    /**
     * @brief Helper method that fetches the next connection address selected by
     * the user/system
     * @return COnnection address
     */
    gt::process_runner::ConnectionAddress getConnectionAddress() const;

private slots:

    /**
     * @brief Handles the notification recieved. Will delete n once used.
     * @param n Notification recieved
     */
    void onNotificationRecieved(GtProcessRunnerNotification* n);

    /**
     * @brief Intercepts the connection state signal
     */
    void onConnectionStateChanged(GtProcessRunnerTransceiver::ConnectionState);

    /**
     * @brief Applies the collected diff to the datamodel
     */
    void onTaskCollected();

    /**
     * @brief Handles the abort signals of the Transceiver
     */
    void onTransceiverAborted();
};

#endif // GTPROCESSRUNNER_H
