/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    explicit GtGraphicsAnimatedPathItem(QGraphicsItem* parent = nullptr);

    int t() const;

    void setT(int t);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

public slots:
    void runPathAnimation();

private:
    qreal m_t;

    QPointer<QPropertyAnimation> m_pathAnim;

};

#endif // GTGRAPHICSANIMATEDPATHITEM_H
