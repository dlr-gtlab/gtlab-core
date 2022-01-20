/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 09.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTGRAPHICSANIMATEDPATHITEM_H
#define GTGRAPHICSANIMATEDPATHITEM_H

#include "gt_gui_exports.h"

#include <QPointer>

#include <QGraphicsPathItem>
#include <QObject>

class QPropertyAnimation;

class GT_GUI_EXPORT GtGraphicsAnimatedPathItem : public QObject,
        public QGraphicsPathItem
{
    Q_OBJECT

    Q_PROPERTY(qreal t READ t WRITE setT)

public:
    GtGraphicsAnimatedPathItem(QGraphicsItem* parent = Q_NULLPTR);

    int t() const;

    void setT(int t);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = 0);

public slots:
    void runPathAnimation();

private:
    qreal m_t;

    QPointer<QPropertyAnimation> m_pathAnim;

};

#endif // GTGRAPHICSANIMATEDPATHITEM_H
