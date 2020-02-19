/* GTlab - Gas Turbine laboratory
 * Source File: gt_processpropertyconnectionentity.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPen>
#include <QApplication>
#include <QMenu>
#include <QIcon>
#include <QDebug>

#include "gt_processpropertyportentity.h"
#include "gt_application.h"
#include "gt_propertyconnection.h"

#include "gt_processpropertyconnectionentity.h"

GtProcessPropertyConnectionEntity::GtProcessPropertyConnectionEntity(
        GtPropertyConnection* connection) :
    m_connection(connection),
    m_startPort(Q_NULLPTR),
    m_endPort(Q_NULLPTR),
    m_previewPort(Q_NULLPTR),
    m_start(QPointF(0., 0.)),
    m_end(QPointF(100., 0.))
{
    setAcceptHoverEvents(true);

    setZValue(-1);

    setPath(getBezierPath(m_start, m_end));
}

GtProcessPropertyConnectionEntity::~GtProcessPropertyConnectionEntity()
{
    if (m_startPort != Q_NULLPTR)
    {
        m_startPort->disconnectPort(this);
    }

    if (m_endPort != Q_NULLPTR)
    {
        m_endPort->disconnectPort(this);
    }
}

bool
GtProcessPropertyConnectionEntity::startPortExists()
{
    if (m_startPort == Q_NULLPTR)
    {
        return false;
    }

    return true;
}

bool
GtProcessPropertyConnectionEntity::endPortExists()
{
    if (m_endPort == Q_NULLPTR)
    {
        return false;
    }

    return true;
}

void
GtProcessPropertyConnectionEntity::setStartPort(
        GtProcessPropertyPortEntity* port)
{
    // check port and connect
    if (port != Q_NULLPTR)
    {
        port->connectPort(this);
    }

    m_startPort = port;
}

void
GtProcessPropertyConnectionEntity::setEndPort(
        GtProcessPropertyPortEntity* port)
{
    // check port and connect
    if (port != Q_NULLPTR)
    {
        port->connectPort(this);
    }

    m_endPort = port;
}

void
GtProcessPropertyConnectionEntity::setStartPoint(const QPointF& point)
{
    m_start = point;
}

void
GtProcessPropertyConnectionEntity::setEndPoint(const QPointF& point)
{
    m_end = point;
}

void
GtProcessPropertyConnectionEntity::setPreviewPort(
        GtProcessPropertyPortEntity* port)
{
    m_previewPort = port;
}

GtProcessPropertyPortEntity*
GtProcessPropertyConnectionEntity::previewPort()
{
    return m_previewPort;
}

QPointF
GtProcessPropertyConnectionEntity::previewPoint()
{
    if (m_previewPort == Q_NULLPTR)
    {
        return QPointF();
    }

    return m_previewPort->pos();
}

void
GtProcessPropertyConnectionEntity::updatePath()
{
    QPointF start = m_start;
    QPointF end = m_end;

    if (m_startPort != Q_NULLPTR)
    {
        start = m_startPort->pos();
    }

    if (m_endPort != Q_NULLPTR)
    {
        end = m_endPort->pos();
    }

    QPen p = pen();
    p.setColor(currentColor());
    setPen(p);

    setPath(getBezierPath(start, end));
}

QPainterPath
GtProcessPropertyConnectionEntity::getBezierPath(QPointF p1, QPointF p2)
{
    QPainterPath pp;

    pp.moveTo(p1);

    qreal dx = (p2.x() - p1.x()) / 4;

    pp.cubicTo(p1.x() + dx, p1.y(),
                 p1.x() + 3 * dx, p2.y(),
                 p2.x(), p2.y());

    return pp;
}

GtPropertyConnection*
GtProcessPropertyConnectionEntity::connection()
{
    return m_connection;
}

void
GtProcessPropertyConnectionEntity::hoverEnterEvent(
        QGraphicsSceneHoverEvent* event)
{
    QPen p = pen();
    p.setColor(Qt::red);
    p.setWidth(2);
    setPen(p);

    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::PointingHandCursor);

    GtGraphicsAnimatedPathItem::hoverEnterEvent(event);
}

void
GtProcessPropertyConnectionEntity::hoverLeaveEvent(
        QGraphicsSceneHoverEvent* event)
{
    QPen p = pen();
    p.setColor(currentColor());
    p.setWidth(1);
    setPen(p);

    QApplication::restoreOverrideCursor();

    GtGraphicsAnimatedPathItem::hoverLeaveEvent(event);
}

void
GtProcessPropertyConnectionEntity::contextMenuEvent(
        QGraphicsSceneContextMenuEvent* event)
{
    qDebug() << "connection context menu!";

    QMenu menu;

    QAction* actdelete = menu.addAction(tr("Delete Connection"));
    actdelete->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

    QAction* a = menu.exec(QCursor::pos());

    if (a == actdelete)
    {
        if (m_connection != Q_NULLPTR)
        {
            if (m_startPort != Q_NULLPTR)
            {
                m_startPort->disconnectPort(this);
            }

            if (m_endPort != Q_NULLPTR)
            {
                m_endPort->disconnectPort(this);
            }

            delete m_connection;
            m_connection = Q_NULLPTR;

            deleteLater();
        }
    }

    GtGraphicsAnimatedPathItem::contextMenuEvent(event);
}

QColor
GtProcessPropertyConnectionEntity::currentColor()
{
    if (m_startPort != Q_NULLPTR && m_endPort != Q_NULLPTR)
    {
        return QColor(Qt::black);
    }

    return QColor(Qt::gray);
}