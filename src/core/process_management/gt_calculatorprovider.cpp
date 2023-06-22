/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorprovider.cpp
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_calculator.h"
#include "gt_objectmemento.h"

#include "gt_calculatorprovider.h"

GtCalculatorProvider::GtCalculatorProvider(GtCalculator* initCalc)
{
    if (initCalc)
    {
        if (initComponent(initCalc->metaObject()))
        {
            setComponentData(initCalc->toMemento());
        }
    }
}
