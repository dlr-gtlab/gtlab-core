/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneHoverEvent>

#include "gt_tasklinkentity.h"
#include "gt_calculator.h"
#include "gt_task.h"
#include "gt_tasklink.h"

GtTaskLinkEntity::GtTaskLinkEntity(GtTask* task, QGraphicsItem* parent) :
    GtCalculatorEntity(task, parent),
    m_taskLink(nullptr)
{

}

GtTaskLinkEntity::GtTaskLinkEntity(GtTaskLink* calc,
                                   GtCalculatorEntity* parent) :
    GtCalculatorEntity(calc, parent),
    m_taskLink(calc)
{

}

void
GtTaskLinkEntity::paint(QPainter* painter,
                        const QStyleOptionGraphicsItem* /*option*/,
                        QWidget* /*widget*/)
{
    if (isRoot())
    {
        return;
    }

    painter->save();

    QPen pen = painter->pen();
    pen.setCosmetic(true);
    painter->setPen(pen);

//    painter->drawRect(boundingRect());




//    QRectF rect(0., 0., m_w, m_h);

//    painter->drawRoundedRect(rect, 5, 5);
//    painter->drawLine(0., 20., m_w, 20.);

    // title
    painter->setBrush(QBrush(QColor(208, 184, 50)));
    QPainterPath titlePath;
    titlePath.setFillRule(Qt::WindingFill);
    titlePath.addRoundedRect(QRect(0, 0, m_w, 20), 5, 5);
    titlePath.addRect(QRect(0, 10, 10, 10));
    titlePath.addRect(QRect(m_w - 10, 10, 10, 10));
    painter->drawPath(titlePath.simplified());

    // footer
    painter->setBrush(QBrush(QColor(248, 233, 133)));
    QPainterPath headerPath;
    headerPath.setFillRule(Qt::WindingFill);
    headerPath.addRoundedRect(QRect(0, 20, m_w, m_h - 20), 5, 5);
    headerPath.addRect(QRect(0, 20, 10, 10));
    headerPath.addRect(QRect(m_w - 10, 20, 10, 10));
    painter->drawPath(headerPath.simplified());

    painter->drawPixmap(2, 2, 16, 16, m_icon);
    painter->drawText(QPoint(25, 14), m_calculator->objectName());

    painter->restore();
}

GtTask*
GtTaskLinkEntity::target()
{
    if (m_taskLink == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return m_taskLink->target();
}

void
GtTaskLinkEntity::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* /*event*/)
{
    setCursor(Qt::ArrowCursor);
    emit switchTo(this);
}

void
GtTaskLinkEntity::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QRectF rect(0., 0., m_w, 20.);

    if (rect.contains(event->pos()))
    {
//        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        setCursor(Qt::OpenHandCursor);
    }
    else
    {
//        QApplication::restoreOverrideCursor();
        QPixmap pixmap(QStringLiteral(":/icons/jumpToIcon.png"));
        pixmap = pixmap.scaledToWidth(30);
        QCursor cursor(pixmap);
        setCursor(cursor);
    }

    QGraphicsObject::hoverMoveEvent(event);
}

