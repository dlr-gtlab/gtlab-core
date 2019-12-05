/* GTlab - Gas Turbine laboratory
 * Source File: gt_scaleproperty.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#include "gt_scaleproperty.h"

GtScaleProperty::GtScaleProperty() :
    m_custom(false)
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
