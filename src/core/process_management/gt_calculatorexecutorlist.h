/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorexecutorlist.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOREXECUTORLIST_H
#define GTCALCULATOREXECUTORLIST_H

#include "gt_core_exports.h"

#include <QObject>
#include <QHash>
#include <QMutex>

#define gtCalcExecList (GtCalculatorExecutorList::instance())

class GtCalculatorExecInterface;
class GtAbstractCalculatorExecutor;
class GtCalculatorExecutorSettings;
class GtAbstractProperty;

/**
 * @brief The GtCalculatorExecutorList class
 */
class GT_CORE_EXPORT GtCalculatorExecutorList : public QObject
{
    Q_OBJECT

    friend class GtProcessModuleLoader;

public:
    /**
     * @brief instance
     * @return
     */
    static GtCalculatorExecutorList* instance();

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
    GtAbstractCalculatorExecutor* executor(const QString& id);

    /**
     * @brief settings
     * @param id
     * @return
     */
    QList<GtAbstractProperty*> settings(const QString& id);

protected:
    /**
     * @brief GtCalculatorExecutorList
     * @param parent
     */
    explicit GtCalculatorExecutorList(QObject* parent = nullptr);

    /**
     * @brief addExecutor
     * @param id
     * @param executor
     * @return
     */
    bool addExecutor(GtCalculatorExecInterface* executor);

    /**
     * @brief generateExecutor
     * @param id
     * @return
     */
    GtAbstractCalculatorExecutor* generateExecutor(const QString& id);

    /**
     * @brief generateSettings
     * @param id
     * @return
     */
    QList<GtAbstractProperty*> generateSettings(const QString& id);

private:
    /// calculator execution objects
    QHash<QString, GtCalculatorExecInterface*> m_calcExecs;

    /// Mutex
    QMutex m_mutex;

};

#endif // GTCALCULATOREXECUTORLIST_H
