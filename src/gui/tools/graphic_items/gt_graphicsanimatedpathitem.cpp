/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 09.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QPropertyAnimation>
#include <QDebug>

#include "gt_graphicsanimatedpathitem.h"
#include "gt_graphicsscene.h"

GtGraphicsAnimatedPathItem::GtGraphicsAnimatedPathItem(QGraphicsItem* parent) :
    QGraphicsPathItem(parent), m_t(0.0), m_pathAnim(nullptr)
{

}

int
GtGraphicsAnimatedPathItem::t() const
{
    return int(m_t);
}

void
GtGraphicsAnimatedPathItem::setT(int t)
{
    m_t = t;
}

void
GtGraphicsAnimatedPathItem::paint(QPainter* painter,
                                  const QStyleOptionGraphicsItem* option,
                                  QWidget* widget)
{
    if (m_pathAnim != Q_NULLPTR)
    {
        painter->save();
        QPen p = pen();
//        QPainterPath painterPath = path().toReversed();

        QPainterPath painterPath = path();
        QVector<qreal> dashes;
        dashes << m_t / p.width() << painterPath.length();
        p.setDashPattern(dashes);
        painter->setPen(p);
        painter->drawPath(painterPath);
        painter->restore();
    }
    else
    {
        QGraphicsPathItem::paint(painter, option, widget);
    }
}

void
GtGraphicsAnimatedPathItem::runPathAnimation()
{
    if (m_pathAnim == Q_NULLPTR)
    {
        m_pathAnim = new QPropertyAnimation(this, "t");
        m_pathAnim->setDuration(300);
        m_pathAnim->setKeyValueAt(0, 0.0);
        m_pathAnim->setKeyValueAt(1, path().length());
        m_pathAnim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

