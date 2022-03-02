/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QPropertyAnimation>

#include "gt_addtaskglobalbutton.h"
#include "gt_taskentity.h"
#include "gt_application.h"
#include "gt_palette.h"

GtAddTaskGlobalButton::GtAddTaskGlobalButton(GtTaskEntity* parent) :
    QGraphicsObject(parent),
    m_w(15),
    m_h(12),
    m_taskEntity(parent),
    m_anim(nullptr)
{
    setAcceptHoverEvents(true);
    setToolTip(tr("Add Global"));
}

void
GtAddTaskGlobalButton::paint(QPainter* painter,
                             const QStyleOptionGraphicsItem* /*option*/,
                             QWidget* /*widget*/)
{
    painter->save();

    if (!gtApp->inDarkMode())
    {
        painter->setBrush(QBrush(Qt::white));
    }
    else
    {
        painter->setBrush(QBrush(GtPalette::basicDarkColor()));
    }

    painter->drawRoundedRect(boundingRect(), 3, 3);
    painter->drawLine(0., -3., 0., 3.);
    painter->drawLine(-3., 0., 3., 0.);

    painter->restore();
}

QRectF
GtAddTaskGlobalButton::boundingRect() const
{
    return QRectF(-m_w / 2, -m_h / 2, m_w, m_h);
}

void
GtAddTaskGlobalButton::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    runEnterAnimation();
    QGraphicsObject::hoverEnterEvent(event);
}

void
GtAddTaskGlobalButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    runLeaveAnimation();
    QGraphicsObject::hoverLeaveEvent(event);
}

void
GtAddTaskGlobalButton::runEnterAnimation()
{
    if (!m_anim)
    {
        m_anim = new QPropertyAnimation(this, "scale");
        m_anim->setDuration(500);
        m_anim->setKeyValueAt(0, 1.0);
        m_anim->setKeyValueAt(0.5, 1.2);
        m_anim->setKeyValueAt(1, 1.1);
        m_anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }

    m_anim->pause();
    m_anim->setKeyValueAt(0, scale());
    m_anim->setKeyValueAt(0.5, 1.2);
    m_anim->setKeyValueAt(1, 1.1);
    m_anim->setCurrentTime(0);
    m_anim->resume();
}

void
GtAddTaskGlobalButton::runLeaveAnimation()
{
    if (!m_anim)
    {
        m_anim = new QPropertyAnimation(this, "scale");
        m_anim->setDuration(500);
        m_anim->setKeyValueAt(0, 1.1);
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
