/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPen>

#include "gt_taskconnectionentity.h"
#include "gt_propertyportentity.h"

GtTaskConnectionEntity::GtTaskConnectionEntity(QGraphicsItem* parent) :
    GtGraphicsAnimatedPathItem(parent)
{
    setOpacity(0.7);
    QPen p = pen();
    p.setColor(QColor(235, 145, 40).darker(150));
    p.setWidth(2);
    setPen(p);
}

void GtTaskConnectionEntity::setFromPort(GtPropertyPortEntity* prop)
{
    m_from = prop;
    updateShape();
}

void GtTaskConnectionEntity::setToPort(GtPropertyPortEntity* prop)
{
    m_to = prop;
    updateShape();
}

void
GtTaskConnectionEntity::updateShape()
{
    if (!m_from)
    {
        return;
    }

    if (!m_to)
    {
        return;
    }

    setPath(createPath(m_from->scenePos(), m_to->scenePos()));
}

QPainterPath
GtTaskConnectionEntity::createPath(const QPointF& p1, const QPointF& p2)
{
    QPainterPath pp;

    // create rect
    QRectF rect(p1.x(), p1.y(), p2.x() - p1.x(), p2.y() - p1.y());

//    if (p1.x() > p2.x())
//    {
//        rect.setX(p2.x());
//        rect.setWidth(p1.x() - p2.x());
//    }
//    else
//    {
//        rect.setX(p1.x());
//        rect.setWidth(p2.x() - p1.x());
//    }

//    if (p1.y() < p2.y())
//    {
//        rect.setY(p1.y());
//        rect.setHeight(p2.y() - p1.y());
//    }
//    else
//    {
//        rect.setY(p2.y());
//        rect.setHeight(p1.y() - p2.y());
//    }


    pp.moveTo(p1);

//    qreal dx = (p2.x() - p1.x()) / 4;

//    pp.cubicTo(p1.x() + dx, p1.y(),
//                 p1.x() + 3 * dx, p2.y(),
//                 p2.x(), p2.y());
    pp.cubicTo(rect.x(), rect.y() + rect.height(),
               rect.x() + rect.width(), rect.y(),
               p2.x(), p2.y());


    return pp;
}

