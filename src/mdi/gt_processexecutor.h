/* GTlab - Gas Turbine laboratory
 * Source File: gt_processexecutor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSEXECUTOR_H
#define GTPROCESSEXECUTOR_H

#include "gt_coreprocessexecutor.h"

#if defined(gtProcessExecutor)
#undef gtProcessExecutor
#endif
#define gtProcessExecutor (static_cast<GtProcessExecutor *>(GtCoreProcessExecutor::instance()))

/**
 * @brief The GtProcessExecutor class
 */
class GtProcessExecutor : public GtCoreProcessExecutor
{
    Q_OBJECT

public:
    GtProcessExecutor(QObject* parent = Q_NULLPTR);

protected:
    /**
     * @brief handleTaskFinishedHelper
     * Uses a loading helper to analyze and apply diff
     *
     * @param changedData
     * @param task
     */
    virtual void handleTaskFinishedHelper(QList<GtObjectMemento>& changedData,
                                          GtTask* task) Q_DECL_OVERRIDE;

    /**
     * @brief execute
     */
    virtual void execute() Q_DECL_OVERRIDE;
private slots:
    /**
     * @brief onHelperFinished - reaction on finished signal of loading helper
     *
     * deletes the loading helper
     */
    void onHelperFinished();

private:
    QString m_taskName;

};

#endif // GTPROCESSEXECUTOR_H