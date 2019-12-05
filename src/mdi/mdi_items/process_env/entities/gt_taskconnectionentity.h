/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKCONNECTIONENTITY_H
#define GTTASKCONNECTIONENTITY_H

#include <QPointer>

#include "gt_graphicsanimatedpathitem.h"

class GtPropertyPortEntity;
/**
 * @brief The GtTaskConnectionEntity class
 */
class GtTaskConnectionEntity : public GtGraphicsAnimatedPathItem
{
    Q_OBJECT

public:
    /**
     * @brief GtTaskConnectionEntity
     * @param parent
     */
    GtTaskConnectionEntity(QGraphicsItem* parent = Q_NULLPTR);

    /**
     * @brief setFromPort
     * @param prop
     */
    void setFromPort(GtPropertyPortEntity* prop);

    /**
     * @brief setToPort
     * @param prop
     */
    void setToPort(GtPropertyPortEntity* prop);

    /**
     * @brief updateShape
     */
    void updateShape();

private:
    /**
     * @brief createPath
     * @param p1
     * @param p2
     * @return
     */
    QPainterPath createPath(const QPointF& p1, const QPointF& p2);

    /**
     * @brief m_in
     */
    QPointer<GtPropertyPortEntity> m_from;

    /**
     * @brief m_out
     */
    QPointer<GtPropertyPortEntity> m_to;

};

#endif // GTTASKCONNECTIONENTITY_H
