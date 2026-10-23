/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processexecinterface.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSEXECINTERFACE_H
#define GT_PROCESSEXECINTERFACE_H

#include "gt_core_exports.h"

#include <QtPlugin>

class GtAbstractProperty;

/**
 * @brief Module interface providing a process executor (execution mode).
 *
 * The interface IID is kept from the former GtCalculatorExecInterface to
 * stay compatible with existing modules.
 */
class GT_CORE_EXPORT GtProcessExecInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy.
     */
    virtual ~GtProcessExecInterface()
    { }

    /**
     * @brief Returns executor identification string .
     * @return Executor identification string.
     */
    virtual QString executorId() = 0;

    /**
     * @brief Returns executor description.
     * @return Executor description.
     */
    virtual QString executorDescription() = 0;

    /**
     * @brief Returns meta object of executor.
     * @return Meta object of executor.
     */
    virtual QMetaObject executor() = 0;

    /**
     * @brief Returns list of executor settings.
     * @return Executor settings.
     */
    virtual QList<GtAbstractProperty*> executorSettings()
    {
        return {};
    };
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtProcessExecInterface,
                    "de.dlr.gtlab.GtCalculatorExecInterface/0.1")
QT_END_NAMESPACE

#endif // GT_PROCESSEXECINTERFACE_H
