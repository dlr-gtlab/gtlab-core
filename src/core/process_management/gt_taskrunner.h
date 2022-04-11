/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskrunner.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKRUNNER_H
#define GTTASKRUNNER_H

#include "gt_core_exports.h"

#include <QObject>
#include <QPointer>
#include <QEventLoop>

#include "gt_objectmemento.h"

class GtTask;
class GtAbstractRunnable;
class GtProcessComponent;
class GtObject;
class GtObjectLinkProperty;

/**
 * @brief The GtTaskRunner class
 */
class GT_CORE_EXPORT GtTaskRunner : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit GtTaskRunner(GtTask* task);

    /**
     * @brief Destructor.
     */
    ~GtTaskRunner() override;

    /**
     * @brief Varifies and sets all necessary parameter.
     * @param Instance of new Runnable.
     * @return Returns true if procedure was successful, otherwise returns
     * false.
     */
    bool setUp(GtAbstractRunnable* runnable, GtObject* source);

    /**
     * @brief Execution process.
     */
    void run();

    /**
     * @brief Returns list of data to merge.
     * @return List of data to merge.
     */
    const QList<GtObjectMemento>& dataToMerge();

private:
    /// pointer to task
    QPointer<GtTask> m_task;

    /// pointer to runnable
    QPointer<GtAbstractRunnable> m_runnable;

    /// source object
    QPointer<GtObject> m_source;

    /// Container constiting of data to merge back into central data model
    QList<GtObjectMemento> m_dataToMerge;

    /// Map of all process components and their cloned counterpart
    QMap<GtProcessComponent*, GtProcessComponent*> m_componentMap;

    /**
     * @brief cloneTask
     * @return
     */
    GtTask* cloneTask();

    /**
     * @brief connectElements
     * @param orig
     * @param cloned
     * @return
     */
    bool setupElements(GtProcessComponent* orig, GtProcessComponent* cloned);

private slots:
    /**
     * @brief handleRunnableFinished
     */
    void handleRunnableFinished();

    /**
     * @brief Transfers all monitoring properties from sender.
     */
    void transferMonitoringProperties();

signals:
    /**
     * @brief Emitted after execution process.
     */
    void finished();

};

#endif // GTTASKRUNNER_H
