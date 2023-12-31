/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKENTITYSHADOW_H
#define GTTASKENTITYSHADOW_H

#include <QGraphicsObject>
#include <QPointer>

class GtTaskEntity;
class QGraphicsDropShadowEffect;
class QPropertyAnimation;

/**
 * @brief The GtTaskEntityShadow class
 */
class GtTaskEntityShadow : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief GtTaskEntityShadow
     * @param task
     */
    explicit GtTaskEntityShadow(GtTaskEntity* task);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /**
    * @brief boundingRect
    * @return
    */
    QRectF boundingRect() const override;

    /**
     * @brief runAnimation
     */
    void runAnimation();

private:
    /// task
    QPointer<GtTaskEntity> m_task;

    ///
    QPointer<QPropertyAnimation> m_anim;

    ///
    QPointer<QGraphicsDropShadowEffect> m_shadow;

};

#endif // GTTASKENTITYSHADOW_H
