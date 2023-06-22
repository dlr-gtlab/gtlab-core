/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcalculatoritem.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSCALCULATORITEM_H
#define GT_PROCESSCALCULATORITEM_H

#include "gt_abstractprocessitem.h"

#include "gt_calculatordata.h"

class GtProcessCategoryItem;

/**
 * @brief The GtProcessCalculatorItem class
 */
class GtProcessCalculatorItem : public GtAbstractProcessItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Calculator data.
     * @param Parent.
     */
    GtProcessCalculatorItem(GtCalculatorData calcData,
                            GtProcessCategoryItem* parent);

    /**
     * @brief Returns calculator data.
     * @return Calculator data.
     */
    GtCalculatorData calculatorData();

private:
    /// Calculator data.
    GtCalculatorData m_calcData;

};

#endif // GT_PROCESSCALCULATORITEM_H
