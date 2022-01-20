/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcalculatoritem.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processcategoryitem.h"

#include "gt_processcalculatoritem.h"

GtProcessCalculatorItem::GtProcessCalculatorItem(
        GtCalculatorData calcData, GtProcessCategoryItem* parent) :
    GtAbstractProcessItem(parent),
    m_calcData(calcData)
{

}

GtCalculatorData
GtProcessCalculatorItem::calculatorData()
{
    return m_calcData;
}
