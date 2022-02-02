/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 06.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKPIPEENTITY_H
#define GTTASKPIPEENTITY_H

#include "gt_graphicsanimatedpathitem.h"

class GtTaskEntity;

/**
 * @brief The GtTaskPipeEntity class
 */
class GtTaskPipeEntity : public GtGraphicsAnimatedPathItem
{
    Q_OBJECT

public:
    /**
     * @brief GtTaskPipeEntity
     * @param parent
     */
    explicit GtTaskPipeEntity(GtTaskEntity* parent);

    /**
     * @brief updateShape
     */
    void updateShape();

private:
    /// Task entity
    GtTaskEntity* m_task;

    /// Arrow head inlet
    QGraphicsPathItem* m_in;

    /// Arror head outlet
    QGraphicsPathItem* m_out;

};

#endif // GTTASKPIPEENTITY_H
