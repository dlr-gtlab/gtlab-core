/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorprovider.cpp
 * copyright 2009-2016 by DLR
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
    if (initCalc != Q_NULLPTR)
    {
        if (initComponent(initCalc->metaObject()))
        {
            setComponentData(initCalc->toMemento());
        }
    }
}
