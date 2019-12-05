/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 06.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QBrush>
#include <QPen>

#include "gt_taskpipeentity.h"

#include "gt_taskentity.h"
#include "gt_calculatorentity.h"

GtTaskPipeEntity::GtTaskPipeEntity(GtTaskEntity* parent) :
    GtGraphicsAnimatedPathItem(parent),
    m_task(parent),
    m_in(Q_NULLPTR),
    m_out(Q_NULLPTR)
{
    QColor col(70, 70, 70);
    QPen p = pen();
    p.setColor(col);
    p.setJoinStyle(Qt::RoundJoin);
    setPen(p);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
}

void
GtTaskPipeEntity::updateShape()
{
    if (m_task == Q_NULLPTR)
    {
        return;
    }

    if (m_in == Q_NULLPTR)
    {
        m_in = new QGraphicsPathItem(this);
        QColor col(0, 150, 0);
        m_in->setBrush(QBrush(col));
        QPen p = m_in->pen();
        p.setColor(col);
        p.setJoinStyle(Qt::RoundJoin);
        m_in->setPen(p);
    }

    if (m_out == Q_NULLPTR)
    {
        m_out = new QGraphicsPathItem(this);
        QColor col(150, 0, 0);
        m_out->setBrush(QBrush(col));
        QPen p = m_out->pen();
        p.setColor(col);
        p.setJoinStyle(Qt::RoundJoin);
        m_out->setPen(p);
    }

    QPointF p1 = QPointF(10., 60.);
    QPointF p2 = QPointF(m_task->boundingRect().width() - 5., 60.);

    QPointF p3(p1.x() - 5., p1.y() - 5.);
    QPointF p4(p1.x() - 5., p1.y() + 5.);
    QPointF p5(p1.x() - 5., p1.y());
    QPointF p6(p5.x() - 5., p5.y() - 5.);
    QPointF p7(p5.x() - 5., p5.y() + 5.);

    QPointF p8(p2.x() - 5., p2.y() - 5.);
    QPointF p9(p2.x() - 5., p2.y() + 5.);
    QPointF p10(p2.x() + 5., p2.y());
    QPointF p11(p2.x(), p2.y() - 5.);
    QPointF p12(p2.x(), p2.y() + 5.);

    QPainterPath headIn;
    QPainterPath headOut;

    headIn.moveTo(p1);
    headIn.lineTo(p3);
    headIn.lineTo(p4);
    headIn.lineTo(p1);
    headIn.lineTo(p5);
    headIn.lineTo(p6);
    headIn.lineTo(p7);
    headIn.lineTo(p5);

    headOut.moveTo(p2);
    headOut.lineTo(p8);
    headOut.lineTo(p9);
    headOut.lineTo(p2);
    headOut.lineTo(p10);
    headOut.lineTo(p11);
    headOut.lineTo(p12);
    headOut.lineTo(p10);

    m_in->setPath(headIn);
    m_out->setPath(headOut);

    GtCalculatorEntity* rootNode = m_task->rootNode();

    if (rootNode != Q_NULLPTR && !rootNode->isLeaf())
    {
        QList<GtCalculatorEntity*> subNodes = rootNode->subNodes();

        QPointF pos1 = subNodes.first()->scenePos();
        QPointF pos2 = subNodes.last()->scenePos();
        QRectF rect2 = subNodes.last()->boundingRect();

        QPointF l1pe(pos1.x(), p1.y());

        QPainterPath path;

        path.moveTo(p1);
        path.lineTo(l1pe);

        path.moveTo(pos2.x() + rect2.width(), p1.y());
        path.lineTo(p2.x() - 5., p1.y());

        setPath(path);

//        QPointF p13(posFrom.x() + rect1.width(), posFrom.y() + 10.);
//        QPointF p14(posTo.x(), posTo.y() + 10.);
//        QPointF p15(posTo.x() - 5., posTo.y() + 5.);
//        QPointF p16(posTo.x() - 5., posTo.y() + 15.);

//        path.moveTo(p13);
//        path.lineTo(p14);

//        head.moveTo(p14);
//        head.lineTo(p15);
//        head.lineTo(p16);
//        head.lineTo(p14);
    }
}

