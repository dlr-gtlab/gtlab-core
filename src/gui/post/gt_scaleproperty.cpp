/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_scaleproperty.h
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#include "gt_scaleproperty.h"

GtScaleProperty::GtScaleProperty() :
    m_custom(false),
    m_max(1),
    m_min(0)
{

}

bool
GtScaleProperty::custom() const
{
    return m_custom;
}

void
GtScaleProperty::setCustom(bool custom)
{
    m_custom = custom;
}

double
GtScaleProperty::max() const
{
    return m_max;
}

void
GtScaleProperty::setMax(double max)
{
    m_max = max;
}

double
GtScaleProperty::min() const
{
    return m_min;
}

void
GtScaleProperty::setMin(double min)
{
    m_min = min;
}
