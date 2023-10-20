/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcalculatoritem.cpp
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
