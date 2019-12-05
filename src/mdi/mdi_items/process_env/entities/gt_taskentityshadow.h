/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    GtTaskEntityShadow(GtTaskEntity* task);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = Q_NULLPTR) Q_DECL_OVERRIDE;

    /**
    * @brief boundingRect
    * @return
    */
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

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
