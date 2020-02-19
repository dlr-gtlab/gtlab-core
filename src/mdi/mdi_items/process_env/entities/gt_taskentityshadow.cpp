/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include "gt_taskentityshadow.h"
#include "gt_taskentity.h"

GtTaskEntityShadow::GtTaskEntityShadow(GtTaskEntity* task) :
    m_task(task),
    m_anim(Q_NULLPTR),
    m_shadow(Q_NULLPTR)
{
    m_shadow = new QGraphicsDropShadowEffect(this);
    m_shadow->setOffset(3.0);
    m_shadow->setBlurRadius(7.0);
    setGraphicsEffect(m_shadow);
    setZValue(-100.);
}

void
GtTaskEntityShadow::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* /*option*/,
                          QWidget* /*widget*/)
{
    painter->save();

    painter->setBrush(QBrush(Qt::white));
    painter->setPen(Qt::NoPen);

    QRectF br = boundingRect();

    QRectF rect(10., 10., br.width() - 20., br.height() - 20.);

    painter->drawRoundedRect(rect, 10, 10);

    painter->restore();
}

QRectF
GtTaskEntityShadow::boundingRect() const
{
    return m_task->boundingRect();
}

void
GtTaskEntityShadow::runAnimation()
{
    if (m_shadow == Q_NULLPTR)
    {
        return;
    }

    if (m_anim != Q_NULLPTR)
    {
        return;
    }

    m_anim = new QPropertyAnimation(m_shadow, "offset");
    m_anim->setDuration(300);
    m_anim->setKeyValueAt(0, QPointF(0.0, 0.0));
    m_anim->setKeyValueAt(1, QPointF(3.0, 3.0));

    m_anim->start(QAbstractAnimation::DeleteWhenStopped);
}
