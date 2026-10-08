/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractcalculatorexecutor.h
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTABSTRACTCALCULATOREXECUTOR_H
#define GTABSTRACTCALCULATOREXECUTOR_H

#include "gt_abstractprocessexecutor.h"
#include "gt_version.h"

GT_REMOVAL_GUARD(2, 2, "Use GtAbstractProcessExecutor instead");

/**
 * @deprecated Use GtAbstractProcessExecutor instead.
 */
using GtAbstractCalculatorExecutor
    GT_DEPRECATED_ATTR(2, 2, "Use GtAbstractProcessExecutor instead")
    = GtAbstractProcessExecutor;

#endif // GTABSTRACTCALCULATOREXECUTOR_H
