/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    explicit GtAddTaskGlobalButton(GtTaskEntity* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /**
    * @brief boundingRect
    * @return
    */
    QRectF boundingRect() const override;

protected:
    /**
     * @brief hoverEnterEvent
     * @param event
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief hoverLeaveEvent
     * @param event
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

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
