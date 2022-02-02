/* GTlab - Gas Turbine laboratory
 * Source File: gt_processpropertyconnectionentity.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSPROPERTYCONNECTIONENTITY_H
#define GTPROCESSPROPERTYCONNECTIONENTITY_H

#include "gt_graphicsanimatedpathitem.h"

#include <QPointer>

class GtPropertyConnection;
class GtProcessPropertyPortEntity;

/**
 * @brief The GtProcessPropertyConnectionEntity class
 */
class GtProcessPropertyConnectionEntity : public GtGraphicsAnimatedPathItem
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessPropertyConnectionEntity
     */
    explicit GtProcessPropertyConnectionEntity(
            GtPropertyConnection* connection = Q_NULLPTR);

    /**
     * @brief Destructor.
     */
    virtual ~GtProcessPropertyConnectionEntity();

    /**
     * @brief Returns true if start port exists, otherwise returns false.
     * @return Whether start port exists or not.
     */
    bool startPortExists();

    /**
     * @brief Returns true if end port exists, otherwise returns false.
     * @return Whether end port exists or not.
     */
    bool endPortExists();

    /**
     * @brief Sets new start port.
     * @param New start port.
     */
    void setStartPort(GtProcessPropertyPortEntity* port);

    /**
     * @brief Sets new end port.
     * @param New end port.
     */
    void setEndPort(GtProcessPropertyPortEntity* port);

    /**
     * @brief Sets new start point.
     * @param New start point.
     */
    void setStartPoint(const QPointF& point);

    /**
     * @brief Sets new end point.
     * @param New end point.
     */
    void setEndPoint(const QPointF& point);

    /**
     * @brief Sets new preview port.
     * @param New preview port.
     */
    void setPreviewPort(GtProcessPropertyPortEntity* port);

    /**
     * @brief Returns pointer to preview port.
     * @return Pointer to preview port.
     */
    GtProcessPropertyPortEntity* previewPort();

    /**
     * @brief Returns position of preview port. If no preview port is set empty
     * point is returned.
     * @return Preview port position.
     */
    QPointF previewPoint();

    /**
     * @brief Updates path based on start and end definition.
     */
    void updatePath();

    /**
     * @brief getBezierPath
     * @param p1
     * @param p2
     * @return
     */
    static QPainterPath getBezierPath(QPointF p1, QPointF p2);

    /**
     * @brief connection
     * @return
     */
    GtPropertyConnection* connection();

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
     * @brief contextMenuEvent
     * @param event
     */
    void contextMenuEvent(
            QGraphicsSceneContextMenuEvent* event) override;

private:
    /// Pointer to connection datamodel object.
    GtPropertyConnection* m_connection;

    /// Pointer to start port entity
    QPointer<GtProcessPropertyPortEntity> m_startPort;

    /// Pointer to end port entity
    QPointer<GtProcessPropertyPortEntity> m_endPort;

    /// Pointer to preview port entity
    QPointer<GtProcessPropertyPortEntity> m_previewPort;

    /// Start point used if no port is set.
    QPointF m_start;

    /// End point used if no port is set.
    QPointF m_end;

    /**
     * @brief Returns color based on current entity state.
     * @return Current color.
     */
    QColor currentColor();

};

#endif // GTPROCESSPROPERTYCONNECTIONENTITY_H
