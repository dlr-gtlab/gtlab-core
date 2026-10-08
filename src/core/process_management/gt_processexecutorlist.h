/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_processexecutorlist.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSEXECUTORLIST_H
#define GT_PROCESSEXECUTORLIST_H

#include "gt_core_exports.h"

#include <QObject>
#include <QHash>
#include <QMutex>

#define gtProcessExecList (GtProcessExecutorList::instance())

class GtProcessExecInterface;
class GtAbstractProcessExecutor;
class GtCalculatorExecutorSettings;
class GtAbstractProperty;

/**
 * @brief Registry of the process executors provided by modules
 */
class GT_CORE_EXPORT GtProcessExecutorList : public QObject
{
    Q_OBJECT

    friend class GtProcessModuleLoader;

public:
    /**
     * @brief instance
     * @return
     */
    static GtProcessExecutorList* instance();

    /**
     * @brief calculatorExecutorIds
     * @return
     */
    QStringList executorIds();

    /**
     * @brief executorExists
     * @param id
     * @return
     */
    bool executorExists(const QString& id);

    /**
     * @brief executor
     * @param id
     * @return
     */
    GtAbstractProcessExecutor* executor(const QString& id);

    /**
     * @brief settings
     * @param id
     * @return
     */
    QList<GtAbstractProperty*> settings(const QString& id);

protected:
    /**
     * @brief GtProcessExecutorList
     * @param parent
     */
    explicit GtProcessExecutorList(QObject* parent = nullptr);

    /**
     * @brief Registers the executor provided by a module.
     * @param executorInterface Module interface that provides the executor
     * @return False if the interface is null or its executor id already exists
     */
    bool addExecutor(GtProcessExecInterface* executorInterface);

    /**
     * @brief generateExecutor
     * @param id
     * @return
     */
    GtAbstractProcessExecutor* generateExecutor(const QString& id);

    /**
     * @brief generateSettings
     * @param id
     * @return
     */
    QList<GtAbstractProperty*> generateSettings(const QString& id);

private:
    /// calculator execution objects
    QHash<QString, GtProcessExecInterface*> m_calcExecs;

    /// Mutex
    QMutex m_mutex;

};

#endif // GT_PROCESSEXECUTORLIST_H
