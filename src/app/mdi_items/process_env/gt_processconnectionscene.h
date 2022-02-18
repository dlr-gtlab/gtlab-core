/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionscene.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCONNECTIONSCENE_H
#define GTPROCESSCONNECTIONSCENE_H

#include "gt_graphicsscene.h"

#include <QPointer>

class QParallelAnimationGroup;
class QPropertyAnimation;
class GtProcessPropertyConnectionEntity;
class GtProcessPropertyPortEntity;
class GtProcessConnectionGraphicsView;

/**
 * @brief The GtProcessConnectionScene class
 */
class GtProcessConnectionScene : public GtGraphicsScene
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    explicit GtProcessConnectionScene(GtProcessConnectionGraphicsView* parent);

    /**
     * @brief Destructor.
     */
    virtual ~GtProcessConnectionScene();

public slots:
    /**
     * @brief Runs an animation based on selected port entity.
     */
    void animatePorts();

    /**
     * @brief Brings Ports back to normal
     */
    void resetPorts();

protected:
    /**
     * @brief mousePressEvent
     * @param mouseEvent
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

    /**
     * @brief mouseReleaseEvent
     * @param mouseEvent
     */
    void mouseReleaseEvent(
            QGraphicsSceneMouseEvent* mouseEvent) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

private:
    /// Pointer ro temporary connection
    GtProcessPropertyConnectionEntity* m_tempConnection;

    /// Pointer to animation group
    QPointer<QParallelAnimationGroup> m_animationGroup;

    /// process connection view
    QPointer<GtProcessConnectionGraphicsView> m_view;

    /**
     * @brief refreshTempConnection
     * @param mousePos
     */
    void refreshTempConnection(const QPointF& mousePos);

    /**
     * @brief Finds all Ports which are possibly connectable to the active port
     * @param activePort currently selected port
     * @return List of valid ports
     */
    QList<GtProcessPropertyPortEntity*> validPorts(
            GtProcessPropertyPortEntity* activePort);

    /**
     * @brief hidePorts
     * @param ports
     * @return
     */
    QList<QPropertyAnimation*> hidePorts(
            const QList<GtProcessPropertyPortEntity*>& ports);

    /**
     * @brief highlightPorts
     * @param ports
     * @return
     */
    QList<QPropertyAnimation*> highlightPorts(
            const QList<GtProcessPropertyPortEntity*>& ports);

    /**
     * @brief Returns closest port corresponding to given port and mouse
     * position. Returns null pointer if no port was found.
     * @param Port entity.
     * @param Mouse position.
     * @return Closest port.
     */
    GtProcessPropertyPortEntity* closestPort(GtProcessPropertyPortEntity* port,
                                             const QPointF& position);

    /**
     * @brief Creates new connection for given ports.
     * @param First connection partner.
     * @param Second connection partner.
     */
    void createNewConnection(GtProcessPropertyPortEntity* portA,
                             GtProcessPropertyPortEntity* portB);

};

#endif // GTPROCESSCONNECTIONSCENE_H
