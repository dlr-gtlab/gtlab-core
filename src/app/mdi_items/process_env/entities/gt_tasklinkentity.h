/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKLINKENTITY_H
#define GTTASKLINKENTITY_H

#include "gt_calculatorentity.h"

class GtTaskLink;

/**
 * @brief The GtTaskLinkEntity class
 */
class GtTaskLinkEntity : public GtCalculatorEntity
{
    Q_OBJECT

public:
    GtTaskLinkEntity(GtTask* task,
                     QGraphicsItem* parent);

    GtTaskLinkEntity(GtTaskLink* calc,
                     GtCalculatorEntity* parent);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = nullptr) override;

    /**
     * @brief target
     * @return
     */
    GtTask* target();

protected:
    /**
     * @brief mouseDoubleClickEvent
     * @param event
     */
    virtual void mouseDoubleClickEvent(
            QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief hoverMoveEvent
     * @param event
     */
    virtual void hoverMoveEvent(
            QGraphicsSceneHoverEvent* event) override;

private:
    /// Pointer to data model member of task link
    GtTaskLink* m_taskLink;

};

#endif // GTTASKLINKENTITY_H
