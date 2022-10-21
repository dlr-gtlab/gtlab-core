/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASK_H
#define GTTASK_H

#include "gt_core_exports.h"

#include <QEventLoop>
#include <QPointer>

#include "gt_processcomponent.h"
#include "gt_objectmemento.h"
#include "gt_intmonitoringproperty.h"
#include "gt_monitoringdatatable.h"

class GtCalculator;


/**
 * @brief The GtTask class
 */
class GT_CORE_EXPORT GtTask : public GtProcessComponent
{
    Q_OBJECT

public:
    enum EVALUATION
    {
        EVAL_OK,
        EVAL_FAILED,
        EVAL_FINISHED
    };

    /**
     * @brief GtTask
     */
    Q_INVOKABLE GtTask();

    /**
     * @brief Main run method of the process component.
     * @return Whether run process was successful or not.
     */
    bool exec() override;

    /**
     * @brief run
     */
    void run(GtAbstractRunnable* runnable);

    /**
     * @brief calculators
     * @return
     */
    QList<GtCalculator*> calculators();

    /**
     * @brief processComponents
     * @return
     */
    QList<GtProcessComponent*> processComponents();

    /**
     * @brief objectsToMerge
     * @return
     */
    QList<GtObjectMemento>& dataToMerge();

    /**
     * @brief Virtual method to implement an initial setup for the iteration
     * process.
     * @return Returns true if setup was successful, otherwise returns false
     */
    virtual bool setUp();

    /**
     * @brief Virtual method to implement an evaluation method for each
     * iteration step.
     * @return Evaluation state for current iteration step
     */
    virtual GtTask::EVALUATION evaluate();

    /**
     * @brief Virtual method to implement an iteration procedure.
     * Default implementation iterates up to the defined maximum iteration
     * number.
     */
    virtual bool runIteration();

    /**
     * @brief Returns size of monitoring data table.
     * @return Size of monitoring data table.
     */
    int monitoringDataSize() const;

    /**
     * @brief Returns reference to monitoring data table.
     * @return Reference to monitoring data table.
     */
    const GtMonitoringDataTable& monitoringDataTable();

    /**
     * @brief Returns number of max. iteration steps.
     * @return Max. number of iteration steps.
     */
    int maxIterationSteps() const;

    /**
     * @brief Returns current iteration step.
     * @return Current iteration step.
     */
    int currentIterationStep() const;

    /**
     * @brief Called after successfully restoring from memento
     */
    void onObjectDataMerged() override;

    /**
     * @brief Collects all property connections recursively.
     * @return List of property connections.
     */
    QList<GtPropertyConnection*> collectPropertyConnections();

    /**
     * @brief Requests a task termination. Task is then stopped
     * after current running Calculator has finished.
     */
    void requestInterruption();

    /**
     * @brief Returns interruption request state.
     * @return Whether current running task is marked for termination or not.
     */
    bool isInterruptionRequested() const;

public slots:
    /**
     * @brief Called when monitoring data is available.
     * @param Iteration number for monitoring data.
     * @param Monitoring data map.
     */
    void onMonitoringDataAvailable(int iteration,
                                   const GtMonitoringDataSet& set);

    /**
     * @brief Clears entire monitoring data.
     */
    void clearMonitoringData();

protected: 
    /// Maximal number of iteration steps
    GtIntProperty m_maxIter;

    /// Current iteration
    GtIntMonitoringProperty m_currentIter;

    /// Last evaluation state
    GtTask::EVALUATION m_lastEval;

    /**
     * @brief Enables the user to specifify the maximum number of iterations
     * in the GUI.
     */
    void enableMaxIterationProperty();

    /**
     * @brief Enables the user to monitor the current iteration number
     * in the GUI.
     */
    void enableCurrentIterationMonitoring();

    /**
     * @brief Starts the execution process of all child process elements.
     * The current iteration number is automatically increased and all
     * monitoring properties transferred. After the successfull run of the
     * elements the evaluation method is called.
     */
    virtual bool runChildElements();

    /**
     * @brief Collects all monitoring data recursively.
     * @return Monitoring data map.
     */
    GtMonitoringDataSet collectMonitoringData();

private:
    /// Event loop
    QEventLoop m_eventLoop;

    /// List of all data to merge
    QList<GtObjectMemento> m_dataToMerge;

    /// Monitoring data table
    GtMonitoringDataTable m_monitoringDataTable;

    /// Interruption flag
    QAtomicInt m_interrupt;

    /**
     * @brief Helper for collecting all monitoring data recursively.
     * @param Monitoring data map reference.
     */
    /**
     * @brief Helper for collecting all monitoring data recursively.
     * @param Monitoring data map reference.
     * @param Process component pointer.
     */
    void collectMonitoringDataHelper(GtMonitoringDataSet& map,
                                     GtProcessComponent* component);

    /**
     * @brief Helper for collecting all property connections recursively.
     * @param Property connection list.
     * @param Process component pointer.
     */
    void collectPropertyConnectionHelper(QList<GtPropertyConnection*>& list,
                                         GtProcessComponent* component);

    /**
     * @brief Returns whether one of the children has warnings.
     * @return True if one of the children has warnings.
     */
    bool childHasWarnings() const;

private slots:
    /**
     * @brief handleRunnableFinished
     */
    void handleRunnableFinished();

signals:
    /**
     * @brief finished
     */
    void finished();

    /**
     * @brief Emitted when monitoring data is available for transfer.
     * @param Iteration number for monitoring data.
     * @param Monitoring data map.
     */
    void monitoringDataTransfer(int iteration,
                                GtMonitoringDataSet map);

    /**
     * @brief Emitted when monitoring data is available.
     */
    void monitoringDataAvailable();

    /**
     * @brief Emitted to trigger a full clear of monitoring data.
     */
    void triggerClearMonitoringData();

};

Q_DECLARE_METATYPE(GtTask*)

#endif // GTTASK_H
