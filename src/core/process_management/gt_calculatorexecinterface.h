/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorexecinterface.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CALCULATOREXECINTERFACE_H
#define GT_CALCULATOREXECINTERFACE_H

#include "gt_processexecinterface.h"
#include "gt_version.h"

GT_REMOVAL_GUARD(2, 2, "Use GtProcessExecInterface instead");

/**
 * @deprecated Use GtProcessExecInterface instead.
 */
using GtCalculatorExecInterface GT_DEPRECATED_ATTR(
    2, 2, "Use GtProcessExecInterface instead") = GtProcessExecInterface;

#endif // GT_CALCULATOREXECINTERFACE_H
