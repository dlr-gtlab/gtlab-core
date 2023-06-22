/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_extendedcalculatordata.cpp
 *
 *  Created on: 13.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_customprocesswizard.h"

#include "gt_extendedcalculatordata.h"

GtExtendedCalculatorDataImpl::GtExtendedCalculatorDataImpl(
        const QMetaObject& metaData) : GtCalculatorDataImpl(metaData),
    wizard(nullptr)
{

}

GtExtendedCalculatorDataImpl::~GtExtendedCalculatorDataImpl()
{
    delete wizard;
}

bool
GtExtendedCalculatorDataImpl::isValid() const
{
    return GtCalculatorDataImpl::isValid();
}

GtExtendedCalculatorData
GtExtendedCalculatorDataImpl::newExtendedCalcData(const QMetaObject& metaData)
{
    return GtExtendedCalculatorData(new GtExtendedCalculatorDataImpl(metaData));
}
