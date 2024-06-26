/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processconnectionscene.cpp
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QPen>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

#include "qmath.h"

#include "gt_processpropertyportentity.h"
#include "gt_processpropertyconnectionentity.h"
#include "gt_processconnectiongraphicsview.h"
#include "gt_propertyconnection.h"
#include "gt_task.h"
#include "gt_colors.h"

#include "gt_processconnectionscene.h"

GtProcessConnectionScene::GtProcessConnectionScene(
        GtProcessConnectionGraphicsView* parent) :
    GtGraphicsScene(parent),
    m_tempConnection(nullptr),
    m_animationGroup(nullptr),
    m_view(parent)
{

}
void
GtProcessConnectionScene::animatePorts()
{
    QList<QGraphicsItem*> itemList = selectedItems();

    if (itemList.size() > 1 || itemList.isEmpty())
    {
        return;
    }

    GtProcessPropertyPortEntity* port =
            dynamic_cast<GtProcessPropertyPortEntity*>(itemList.first());

    // check port
    if (!port)
    {
        return;
    }

    if (m_animationGroup)
    {
        m_animationGroup->stop();

        delete m_animationGroup;
        m_animationGroup = nullptr;
    }

    QList<GtProcessPropertyPortEntity*> valids = validPorts(port);
    QList<GtProcessPropertyPortEntity*> invalids =
            findItems<GtProcessPropertyPortEntity*>();

    invalids.removeOne(port);

    foreach (GtProcessPropertyPortEntity* currPort, valids)
    {
        if (invalids.contains(currPort))
        {
            invalids.removeAll(currPort);
        }
    }

    m_animationGroup = new QParallelAnimationGroup(this);

    foreach (QPropertyAnimation* anim, highlightPorts(valids))
    {
        m_animationGroup->addAnimation(anim);
    }

    foreach (QPropertyAnimation* anim, hidePorts(invalids))
    {
        m_animationGroup->addAnimation(anim);
    }

    m_animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void
GtProcessConnectionScene::resetPorts()
{
    QList<GtProcessPropertyPortEntity*> ports =
            findItems<GtProcessPropertyPortEntity*>();

    if (m_animationGroup)
    {
        m_animationGroup->stop();
        delete m_animationGroup;
    }

    m_animationGroup = new QParallelAnimationGroup(this);


    foreach (GtProcessPropertyPortEntity* port, ports)
    {
        m_animationGroup->addAnimation(port->resetPortAnim());
    }

     m_animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void
GtProcessConnectionScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    GtGraphicsScene::mousePressEvent(mouseEvent);
}

void
GtProcessConnectionScene::mouseReleaseEvent(
        QGraphicsSceneMouseEvent* mouseEvent)
{
    if (m_tempConnection)
    {
        // check for connection partner
        GtProcessPropertyPortEntity* closePort =
                closestPort(m_tempConnection->previewPort(),
                            mouseEvent->scenePos());

        if (closePort)
        {
            createNewConnection(m_tempConnection->previewPort(),
                                closePort);
        }

        delete m_tempConnection;
        m_tempConnection = nullptr;

        resetPorts();
    }

//    resetPorts();

    GtGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void
GtProcessConnectionScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QList<QGraphicsItem*> itemList = selectedItems();

    if (event->buttons() & Qt::LeftButton)
    {
        if (itemList.isEmpty())
        {
            return;
        }

        if (itemList.size() > 1)
        {
            return;
        }

        if (!m_tempConnection)
        {
            GtProcessPropertyPortEntity* port =
                    dynamic_cast<GtProcessPropertyPortEntity*>(
                        itemList.first());

            if (port)
            {
                m_tempConnection = new GtProcessPropertyConnectionEntity();
                QPen pen = m_tempConnection->pen();
                pen.setColor(gt::gui::color::connection_editor::connectionDraft());
                m_tempConnection->setPen(pen);
                m_tempConnection->setZValue(-0.5);

                m_tempConnection->setPath(
                            m_tempConnection->getBezierPath(port->pos(),
                                                            port->pos()));

                m_tempConnection->setPreviewPort(port);

                addItem(m_tempConnection);
            }
        }
        else
        {
            GtProcessPropertyPortEntity* closePort =
                    closestPort(m_tempConnection->previewPort(),
                                event->scenePos());

            if (closePort)
            {
                refreshTempConnection(closePort->pos());
            }
            else
            {
                refreshTempConnection(event->scenePos());
            }
        }

//        refreshTempConnection(event->scenePos());
    }

    GtGraphicsScene::mouseMoveEvent(event);
}

void
GtProcessConnectionScene::refreshTempConnection(const QPointF& mousePos)
{
    if (!m_tempConnection)
    {
        return;
    }

    QPointF firstPoint = m_tempConnection->previewPoint();

    m_tempConnection->setPath(GtProcessPropertyConnectionEntity::getBezierPath(
                                  firstPoint, mousePos));
}

QList<GtProcessPropertyPortEntity*>
GtProcessConnectionScene::validPorts(GtProcessPropertyPortEntity* activePort)
{
    // check active port
    if (!activePort)
    {
        return QList<GtProcessPropertyPortEntity*>();
    }

    QList<GtProcessPropertyPortEntity*> retval =
            findItems<GtProcessPropertyPortEntity*>();

    retval.removeOne(activePort);

    if (activePort->portType() == GtProcessPropertyPortEntity::OUTPUT_PORT)
    {
        // output port type
        foreach (GtProcessPropertyPortEntity* port, retval)
        {
            if (port->portType() == GtProcessPropertyPortEntity::OUTPUT_PORT)
            {
                retval.removeOne(port);
            }
        }
    }
    else
    {
        // input port type
        foreach (GtProcessPropertyPortEntity* port, retval)
        {
            if (port->portType() == GtProcessPropertyPortEntity::INPUT_PORT)
            {
                retval.removeOne(port);
            }
        }
    }

    // check remaining ports
    foreach (GtProcessPropertyPortEntity* port, retval)
    {
        if (!activePort->canConnect(port))
        {
            retval.removeOne(port);
        }
    }

    return retval;
}

QList<QPropertyAnimation*>
GtProcessConnectionScene::hidePorts(
        const QList<GtProcessPropertyPortEntity*>& ports)
{
    QList<QPropertyAnimation*> retval;

    foreach (GtProcessPropertyPortEntity* port, ports)
    {
        QPropertyAnimation* anim = port->hidePortAnim();

        if (anim)
        {
            retval << anim;
        }
    }

    return retval;
}

QList<QPropertyAnimation*>
GtProcessConnectionScene::highlightPorts(
        QList<GtProcessPropertyPortEntity*> const& ports)
{
    QList<QPropertyAnimation*> animList;

    foreach (GtProcessPropertyPortEntity* port, ports)
    {
        QPropertyAnimation* anim = port->highlightPortAnim();

        if (anim)
        {
            animList.append(anim);
        }
    }

    return animList;
}

GtProcessPropertyPortEntity*
GtProcessConnectionScene::closestPort(GtProcessPropertyPortEntity* port,
                                      const QPointF& position)
{
    QList<GtProcessPropertyPortEntity*> ports = validPorts(port);

    foreach (GtProcessPropertyPortEntity* currentPort, ports)
    {
        double dx = qAbs(position.x() - currentPort->scenePos().x());
        double dy = qAbs(position.y() - currentPort->scenePos().y());

        if (qSqrt(dx * dx + dy * dy) < 15.0)
        {
            return currentPort;
        }
    }

    return nullptr;
}

void
GtProcessConnectionScene::createNewConnection(
        GtProcessPropertyPortEntity* portA,
        GtProcessPropertyPortEntity* portB)
{
    // check ports
    if (!portA || !portB)
    {
        return;
    }

    // check view
    if (!m_view)
    {
        return;
    }

    // check root task
    if (!m_view->root())
    {
        return;
    }

    GtProcessPropertyPortEntity* source = nullptr;
    GtProcessPropertyPortEntity* target = nullptr;

    if (portA->portType() == GtProcessPropertyPortEntity::INPUT_PORT)
    {
        target = portA;
        source = portB;
    }
    else
    {
        target = portB;
        source = portA;
    }

    QString sourceUuid = source->parentComponentUuid();
    QString sourceProp = source->propertyId();
    QString targetUuid = target->parentComponentUuid();
    QString targetProp = target->propertyId();

    GtPropertyConnection* connection = new GtPropertyConnection;
    connection->setObjectName(connection->uuid());

    connection->setSourceUuid(sourceUuid);
    connection->setSourceProp(sourceProp);
    connection->setTargetUuid(targetUuid);
    connection->setTargetProp(targetProp);

    m_view->root()->appendChild(connection);
    connection->makeConnection();

    GtProcessPropertyConnectionEntity* entity =
            m_view->createConnection(connection);

    if (entity)
    {
        m_view->updateConnections();
//        entity->runPathAnimation();
    }
}
