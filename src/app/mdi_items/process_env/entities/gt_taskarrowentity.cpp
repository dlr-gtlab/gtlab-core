/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QBrush>
#include <QPen>
#include <QDebug>

#include "gt_taskarrowentity.h"
#include "gt_calculatorentity.h"
#include "gt_taskarrowlabelentity.h"
#include "gt_object.h"

GtTaskArrowEntity::GtTaskArrowEntity(ArrowTypes type,
                                     GtCalculatorEntity* parent) :
    GtGraphicsAnimatedPathItem(parent),
    m_type(type),
    m_from(nullptr),
    m_to(nullptr),
    m_parent(parent),
    m_head(nullptr)
{
    QColor col(70, 70, 70);
    QPen p = pen();
    p.setColor(col);
    p.setJoinStyle(Qt::RoundJoin);
    setPen(p);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
}

void GtTaskArrowEntity::link(GtCalculatorEntity* from, GtCalculatorEntity* to)
{
    m_from = from;
    m_to = to;

//    updateShape();
}

void
GtTaskArrowEntity::updateShape()
{
    if (!m_from || !m_to)
    {
        return;
    }

    if (!m_head)
    {
        m_head = new QGraphicsPathItem(this);
        QColor col(70, 70, 70);
        m_head->setBrush(QBrush(col));
        QPen p = m_head->pen();
        p.setColor(col);
        p.setJoinStyle(Qt::RoundJoin);
        m_head->setPen(p);
    }

    QPainterPath path;
    QPainterPath head;

//    qDebug() << "GtTaskArrowEntity::updateShape() - " << m_from->toString();

    QRectF rect1 = m_from->boundingRect();
    QRectF rect2 = m_to->boundingRect();

    QPointF posFrom = m_from->pos();
    QPointF posTo = m_to->pos();

    if (m_type == ARROW_DIRECT)
    {
        QPointF p1(posFrom.x() + rect1.width(), posFrom.y() + 10.);
        QPointF p2(posTo.x(), posTo.y() + 10.);
        QPointF p3(posTo.x() - 5., posTo.y() + 5.);
        QPointF p4(posTo.x() - 5., posTo.y() + 15.);

        path.moveTo(p1);
        path.lineTo(p2);

        head.moveTo(p2);
        head.lineTo(p3);
        head.lineTo(p4);
        head.lineTo(p2);
    }
    else if (m_type == ARROW_LEFT_CHILD)
    {
        QPointF p1(20., rect1.height());
        QPointF p2(20., rect1.height() + 10.);
        QPointF p3(posTo.x() - 20., posTo.y() - 10.);
        QPointF p4(posTo.x() - 20., posTo.y() + 10.);
        QPointF p5(posTo.x(), posTo.y() + 10.);
        QPointF p6(posTo.x() - 5., posTo.y() + 5.);
        QPointF p7(posTo.x() - 5., posTo.y() + 15.);

        path.moveTo(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.lineTo(p4);
        path.lineTo(p5);

        head.moveTo(p5);
        head.lineTo(p6);
        head.lineTo(p7);
        head.lineTo(p5);
    }
    else
    {
        QPointF p1(posFrom.x() + rect1.width(), posFrom.y() + 10.);
        QPointF p2(posFrom.x() + rect1.width() + 20., posFrom.y() + 10.);
        QPointF p3(posFrom.x() + rect1.width() + 20., posFrom.y() - 10.);
        QPointF p4(rect2.width() - 20., rect2.height() + 10.);
        QPointF p5(rect2.width() - 20., rect2.height());
        QPointF p6(rect2.width() - 25., rect2.height() + 5.);
        QPointF p7(rect2.width() - 15., rect2.height() + 5.);

        path.moveTo(p1);
        path.lineTo(p2);
        path.lineTo(p3);
        path.lineTo(p4);
        path.lineTo(p5);

        head.moveTo(p5);
        head.lineTo(p6);
        head.lineTo(p7);
        head.lineTo(p5);
    }

    setPath(path);
    m_head->setPath(head);
}

void
GtTaskArrowEntity::setToPreviewMode()
{
//    qDebug() << "GtTaskArrowEntity::setToPreviewMode";
    GtTaskArrowLabelEntity* label = new GtTaskArrowLabelEntity(this);

    connect(label, SIGNAL(addNewItem(GtObject*)), SLOT(onItemAdd(GtObject*)));

    QPointF pp = path().pointAtPercent(0.5);
    QRectF br = label->boundingRect();
    label->setPos(pp.x() - br.width() / 2, pp.y() - br.height() / 2);
}

void
GtTaskArrowEntity::onItemAdd(GtObject* obj)
{
    if (obj)
    {
        if (m_parent)
        {
            m_parent->addChild(obj);
        }
        else
        {
            delete obj;
            qDebug() << "GtTaskArrowEntity::onItemAdd";
        }
    }
}

