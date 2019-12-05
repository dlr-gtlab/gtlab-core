/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcomponent.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 29.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTPROCESSCOMPONENT_H
#define GTPROCESSCOMPONENT_H

#include "gt_calculators_exports.h"
#include "gt_object.h"

class GtTask;

/**
 * @brief The GtProcessComponent class
 */
class GT_CALCULATORS_EXPORT  GtProcessComponent : public GtObject
{
    Q_OBJECT

public:
    enum STATE
    {
        NONE = 0,
        RUNNING, // 1
        QUEUED, // 2
        FAILED, // 3
        FINISHED, // 4
        WARN_FINISHED, // 5
        CONNECTING, // 6
        SKIPPED, // 7
        TERMINATION_REQUESTED, // 8
        TERMINATED // 9
    };

    /**
     * @brief Main execution method of the process component.
     * @return Whether execution procedure was successful or not.
     */
    virtual bool exec() = 0;

    /**
     * @brief Returns current state of the process component.
     * @return Current state of process component.
     */
    GtProcessComponent::STATE currentState();

    /**
     * @brief Sets current state of process component.
     * @param New state.
     */
    virtual void setState(GtProcessComponent::STATE state);

    /**
     * @brief Sets current state of process component and all his child
     * elements recursively.
     * @param New state.
     */
    void setStateRecursively(GtProcessComponent::STATE state);

    /**
     * @brief Returns list of all monitoring properties.
     * @return List of monitoring properties.
     */
    const QList<GtAbstractProperty*>& monitoringProperties();

    /**
     * @brief Returns list of all read and write properties.
     * @return List of read and write properties.
     */
    QList<GtAbstractProperty*> readWriteProperties();

    /**
     * @brief Returns list of all non read only properties.
     * @return List of non read only properties.
     */
    QList<GtAbstractProperty*> nonReadOnlyProperties();

    /**
     * @brief Resets all monitoring properties recursively for whole process
     * component tree.
     */
    void resetMonitoringProperties();

    /**
     * @brief Returns root task of current process component. Null pointer will
     * be returned if no root task was found.
     * @return Root task.
     */
    GtTask* rootTask();

    /**
     * @brief isReady
     * @return
     */
    bool isReady();

    /**
     * @brief Returns state of warning flag.
     * @return Warning flag.
     */
    bool hasWarnings();

public slots:
    /**
     * @brief Handles process component state changes.
     * @param New state.
     */
    void handleStateChanged(GtProcessComponent::STATE state);

protected:
    /**
     * @brief Constructor
     */
    GtProcessComponent();

    /**
     * @brief registerMonitoringProperty
     * @param property
     */
    void registerMonitoringProperty(GtAbstractProperty& property);

    /**
     * @brief Sets warning flag.
     * @param val New warning flag state.
     */
    void setWarningFlag(bool val);

private:
    /// Current process component state
    GtProcessComponent::STATE m_state;

    /// Monitoring properties
    QList<GtAbstractProperty*> m_monitorProperties;

    /// Warning flag
    bool m_warning;

signals:
    /**
     * @brief Emitted on state change.
     */
    void stateChanged(GtProcessComponent::STATE);

    /**
     * @brief Emitted to transfer monitoring properties.
     */
    void transferMonitoringProperties();

};

Q_DECLARE_METATYPE(GtProcessComponent::STATE);

#endif // GTPROCESSCOMPONENT_H
