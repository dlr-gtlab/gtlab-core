/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_abstractdatazone.h"

GtAbstractDataZone::GtAbstractDataZone()
{

}

const QStringList&
GtAbstractDataZone::params() const
{
    // has to be the same as table()->tabValsKeys();
    return m_params;
}

void
GtAbstractDataZone::setParams(const QStringList &params)
{
    m_params = params;
    changed();
}

const QStringList&
GtAbstractDataZone::units() const
{
    // has to be the same as table()->tabValsKeys();
    return m_units;
}

void
GtAbstractDataZone::setUnits(const QStringList &units)
{
    m_units = units;
    changed();
}
