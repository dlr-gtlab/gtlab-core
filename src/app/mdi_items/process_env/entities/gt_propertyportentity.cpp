/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QPropertyAnimation>

#include "gt_propertyportentity.h"

GtPropertyPortEntity::GtPropertyPortEntity(QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_fillColor(QColor(90, 90, 150)),
    m_anim(nullptr)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
}

void
GtPropertyPortEntity::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* /*option*/,
                            QWidget* /*widget*/)
{
    painter->save();

    painter->setBrush(QBrush(m_fillColor));
    painter->drawEllipse(QPoint(0, 0), 4, 4);

    painter->restore();
}

QRectF
GtPropertyPortEntity::boundingRect() const
{
    return QRectF(-6, -6, 12, 12);
}

void
GtPropertyPortEntity::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    runEnterAnimation();
    QGraphicsObject::hoverEnterEvent(event);
}

void
GtPropertyPortEntity::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    runLeaveAnimation();
    QGraphicsObject::hoverLeaveEvent(event);
}

void
GtPropertyPortEntity::runEnterAnimation()
{
    if (!m_anim)
    {
        m_anim = new QPropertyAnimation(this, "scale");
        m_anim->setDuration(500);
        m_anim->setKeyValueAt(0, 1.0);
        m_anim->setKeyValueAt(0.5, 1.5);
        m_anim->setKeyValueAt(1, 1.2);
        m_anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }

    m_anim->pause();
    m_anim->setKeyValueAt(0, scale());
    m_anim->setKeyValueAt(0.5, 1.5);
    m_anim->setKeyValueAt(1, 1.2);
    m_anim->setCurrentTime(0);
    m_anim->resume();
}

void
GtPropertyPortEntity::runLeaveAnimation()
{
    if (!m_anim)
    {
        m_anim = new QPropertyAnimation(this, "scale");
        m_anim->setDuration(500);
        m_anim->setKeyValueAt(0, 1.2);
        m_anim->setKeyValueAt(1, 1.0);
        m_anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }

    m_anim->pause();
    m_anim->setKeyValueAt(0, scale());
    m_anim->setKeyValueAt(1, 1.0);
    m_anim->setCurrentTime(0);
    m_anim->resume();
}
