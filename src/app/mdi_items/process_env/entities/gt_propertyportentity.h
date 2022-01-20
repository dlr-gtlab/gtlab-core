/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYPORTENTITY_H
#define GTPROPERTYPORTENTITY_H

#include <QGraphicsObject>
#include <QPointer>

class QPropertyAnimation;

/**
 * @brief The GtPropertyPortEntity class
 */
class GtPropertyPortEntity : public QGraphicsObject
{
    Q_OBJECT

public:
    GtPropertyPortEntity(QGraphicsItem* parent = Q_NULLPTR);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = Q_NULLPTR) Q_DECL_OVERRIDE;

    /**
    * @brief boundingRect
    * @return
    */
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    ///
    QColor m_fillColor;

    /**
     * @brief hoverEnterEvent
     * @param event
     */
    virtual void hoverEnterEvent(
            QGraphicsSceneHoverEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief hoverLeaveEvent
     * @param event
     */
    virtual void hoverLeaveEvent(
            QGraphicsSceneHoverEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief runEnterAnimation
     */
    void runEnterAnimation();

    /**
     * @brief runLeaveAnimation
     */
    void runLeaveAnimation();

private:
    ///
    QPointer<QPropertyAnimation> m_anim;

};

#endif // GTPROPERTYPORTENTITY_H
