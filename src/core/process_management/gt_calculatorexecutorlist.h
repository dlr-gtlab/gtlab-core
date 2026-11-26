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

#include "gt_processexecutorlist.h"
#include "gt_version.h"

GT_REMOVAL_GUARD(2, 2, "Use GtProcessExecutorList instead");

/**
 * @deprecated Use GtProcessExecutorList instead.
 */
using GtCalculatorExecutorList GT_DEPRECATED_ATTR(
    2, 2, "Use GtProcessExecutorList instead") = GtProcessExecutorList;

/// @deprecated Use gtProcessExecList instead.
/// Expands to the deprecated alias, so every use emits a warning.
#define gtCalcExecList (GtCalculatorExecutorList::instance())

#endif // GTCALCULATOREXECUTORLIST_H
