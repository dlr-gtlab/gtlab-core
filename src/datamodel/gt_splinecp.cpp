/* GTlab - Gas Turbine laboratory
 * Source File: gt_splinecp.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPointF>

#include "gt_splinecp.h"

GtSplineCP::GtSplineCP() :
    m_x(0.0),
    m_y(0.0)
{
    setObjectName(tr("Control Point"));

    connect(this, SIGNAL(pointChanged()), SLOT(changed()));
}

GtSplineCP::GtSplineCP(const QPointF& cp) :
    m_x(cp.x()),
    m_y(cp.y())
{
    setObjectName(tr("Control Point"));

    connect(this, SIGNAL(pointChanged()), SLOT(changed()));
}

QPointF
GtSplineCP::point()
{
    return QPointF(m_x, m_y);
}

double
GtSplineCP::x()
{
    return m_x;
}

double
GtSplineCP::y()
{
    return m_y;
}

void
GtSplineCP::setValue(const QPointF& val)
{
    m_x = val.x();
    m_y = val.y();

    emit changed();
}
