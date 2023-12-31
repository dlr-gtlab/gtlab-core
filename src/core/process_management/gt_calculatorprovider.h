/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorprovider.h
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORPROVIDER_H
#define GTCALCULATORPROVIDER_H

#include "gt_core_exports.h"

#include "gt_abstractprocessprovider.h"

class GtCalculator;

/**
 * @brief The GtCalculatorProvider class
 */
class GT_CORE_EXPORT GtCalculatorProvider :
        public GtAbstractProcessProvider
{
public:
    /**
     * @brief Constructor.
     * @param initCalc Calculator for provider initialization
     */
    explicit GtCalculatorProvider(GtCalculator* initCalc = nullptr);

};

#endif // GTCALCULATORPROVIDER_H
