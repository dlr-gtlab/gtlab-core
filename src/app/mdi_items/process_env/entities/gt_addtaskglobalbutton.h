/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTADDTASKGLOBALBUTTON_H
#define GTADDTASKGLOBALBUTTON_H

#include <QGraphicsObject>
#include <QPointer>

class GtTaskEntity;
class QPropertyAnimation;

/**
 * @brief The GtAddTaskGlobalButton class
 */
class GtAddTaskGlobalButton : public QGraphicsObject
{
public:
    /**
     * @brief GtAddTaskGlobalButton
     * @param parent
     */
    explicit GtAddTaskGlobalButton(GtTaskEntity* parent = Q_NULLPTR);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = Q_NULLPTR) override;

    /**
    * @brief boundingRect
    * @return
    */
    virtual QRectF boundingRect() const override;

protected:
    /**
     * @brief hoverEnterEvent
     * @param event
     */
    virtual void hoverEnterEvent(
            QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief hoverLeaveEvent
     * @param event
     */
    virtual void hoverLeaveEvent(
            QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief runEnterAnimation
     */
    void runEnterAnimation();

    /**
     * @brief runLeaveAnimation
     */
    void runLeaveAnimation();

private:
    /// width
    int m_w;

    /// height
    int m_h;

    /// Task entity
    GtTaskEntity* m_taskEntity;

    ///
    QPointer<QPropertyAnimation> m_anim;

};

#endif // GTADDTASKGLOBALBUTTON_H
