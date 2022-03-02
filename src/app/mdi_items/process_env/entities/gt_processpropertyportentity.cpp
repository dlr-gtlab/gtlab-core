/* GTlab - Gas Turbine laboratory
 * Source File: gt_processpropertyportentity.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QBrush>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QStyleOptionGraphicsItem>

#include "gt_processconnectionscene.h"
#include "gt_processconnectionitem.h"
#include "gt_processpropertyportentity.h"

#include "gt_application.h"
#include "gt_palette.h"
#include "gt_logging.h"

GtProcessPropertyPortEntity::GtProcessPropertyPortEntity(
        double x, double y, double width, double height, PortTypes typ,
        GtProcessConnectionItem* item) :
    QGraphicsEllipseItem(x, y , width, height),
    m_scale(1.0),
    m_type(typ),
    m_anim(nullptr),
    m_timer(nullptr),
    m_item(item)
{
    setBrush(QBrush(Qt::darkGray));
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
}

GtProcessPropertyPortEntity::~GtProcessPropertyPortEntity()
{
    if (m_timer)
    {
        delete m_timer;
    }
}

bool
GtProcessPropertyPortEntity::connectPort(
        GtProcessPropertyConnectionEntity* connection)
{
    // check connection
    if (!connection)
    {
        return false;
    }

    // check whether connection already exists
    if (m_connections.contains(connection))
    {
        return false;
    }

    // check port type
    if (m_type == GtProcessPropertyPortEntity::INPUT_PORT)
    {
        // only one connection supported - check list size
        if (!m_connections.isEmpty())
        {
            // one or more connections exists already
            return false;
        }
    }

    // add connection
    m_connections << connection;

    // update shape
    updateShape();

    // return success
    return true;
}

bool
GtProcessPropertyPortEntity::disconnectPort(
        GtProcessPropertyConnectionEntity* connection)
{
    // initialize return value
    bool retval = false;

    // remove connection
    retval = m_connections.removeOne(connection);

    // update shape
    updateShape();

    // return value
    return retval;
}

bool
GtProcessPropertyPortEntity::isConnected()
{
    return !m_connections.isEmpty();
}

GtProcessPropertyPortEntity::PortTypes
GtProcessPropertyPortEntity::portType()
{
    return m_type;
}

QPropertyAnimation*
GtProcessPropertyPortEntity::hidePortAnim()
{
    if (m_anim)
    {
        m_anim->stop();
    }

    m_anim = new QPropertyAnimation(this, "scale");
    m_anim->setDuration(500);
    m_anim->setKeyValueAt(0, 1.0);
    m_anim->setKeyValueAt(0.5, 0.2);
    m_anim->setKeyValueAt(1.0, 0.4);

    return m_anim;
}

QPropertyAnimation*
GtProcessPropertyPortEntity::highlightPortAnim()
{
    if (m_anim)
    {
        m_anim->stop();
    }

    m_anim = new QPropertyAnimation(this, "scale");
    m_anim->setDuration(500);
    m_anim->setKeyValueAt(0, 1.0);
    m_anim->setKeyValueAt(0.5, 1.2);
    m_anim->setKeyValueAt(1.0, 1.0);

    return m_anim;
}

QPropertyAnimation*
GtProcessPropertyPortEntity::resetPortAnim()
{
    if (m_anim)
    {
        m_anim->stop();
    }

    m_anim = new QPropertyAnimation(this, "scale");
    m_anim->setDuration(300);
    m_anim->setKeyValueAt(0, scale());
    m_anim->setKeyValueAt(1.0, 1.0);

    return m_anim;
}

bool
GtProcessPropertyPortEntity::canConnect(GtProcessPropertyPortEntity* port)
{
    // check port
    if (!port)
    {
        return false;
    }

    if (port == this)
    {
        return false;
    }

    if (port->portType() == portType())
    {
        return false;
    }

    if (m_type == GtProcessPropertyPortEntity::INPUT_PORT)
    {
        if (isConnected())
        {
            return false;
        }
    }
    else
    {
        if (port->isConnected())
        {
            return false;
        }
    }

    if (propertyValue().typeName() != port->propertyValue().typeName())
    {
        return false;
    }

    if (propertyValue().typeName() == QStringLiteral("QString"))
    {
        if (propertyClassName() != port->propertyClassName())
        {
            return false;
        }
    }

    if (parentComponentUuid() == port->parentComponentUuid())
    {
        return false;
    }

    return true;
}

QVariant
GtProcessPropertyPortEntity::propertyValue()
{
    if (!m_item)
    {
        return QVariant();
    }

    return m_item->propertyValue();
}

QString
GtProcessPropertyPortEntity::propertyClassName()
{
    if (!m_item)
    {
        return QString();
    }

    return m_item->propertyClassName();
}

QString
GtProcessPropertyPortEntity::parentComponentUuid()
{
    if (!m_item)
    {
        return QString();
    }

    return m_item->parentComponentUuid();
}

QString
GtProcessPropertyPortEntity::propertyId()
{
    if (!m_item)
    {
        return QString();
    }

    return m_item->propertyId();
}

void
GtProcessPropertyPortEntity::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (!gtApp->inDarkMode())
    {
        setBrush(QBrush(Qt::white));
    }
    else
    {
        setBrush(QBrush(GtPalette::basicDarkColor()));
    }

    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(Qt::OpenHandCursor);

    runAnimation();
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void
GtProcessPropertyPortEntity::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setBrush(QBrush(Qt::darkGray));
    QApplication::restoreOverrideCursor();
    runAnimation(false);
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void
GtProcessPropertyPortEntity::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        if (!scene())
        {
            return;
        }

        scene()->setFocus();
        setSelected(true);

        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);

        GtProcessConnectionScene* pcScene =
                qobject_cast<GtProcessConnectionScene*>(scene());

        if (!pcScene)
        {
            return;
        }

        if (!m_timer)
        {
            m_timer = new QTimer;
            m_timer->setSingleShot(true);
            m_timer->setInterval(100);

            connect(m_timer.data(), &QTimer::timeout,
                    pcScene, &GtProcessConnectionScene::animatePorts);
        }

        m_timer->start();
    }
}

void
GtProcessPropertyPortEntity::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_timer)
    {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    GtProcessConnectionScene* pcScene =
            qobject_cast<GtProcessConnectionScene*>(scene());

    if (pcScene)
    {
        pcScene->clearSelection();
        pcScene->resetPorts();
    }

    QApplication::restoreOverrideCursor();

    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void
GtProcessPropertyPortEntity::paint(QPainter* painter,
                                   const QStyleOptionGraphicsItem* option,
                                   QWidget* widget)
{
    QStyleOptionGraphicsItem myOption(*option);

    // disable drawing of selection rectangle
    myOption.state &= ~QStyle::State_Selected;

    QGraphicsEllipseItem::paint(painter, &myOption, widget);
}

void
GtProcessPropertyPortEntity::runAnimation(bool enter)
{
    if (m_anim)
    {
        if (enter)
        {
            m_anim->pause();
            m_anim->setKeyValueAt(0, scale());
            m_anim->setKeyValueAt(1, 1.3);
            m_anim->setCurrentTime(0);
            m_anim->resume();
        }
        else
        {
            m_anim->pause();
            m_anim->setKeyValueAt(0, scale());
            m_anim->setKeyValueAt(1, 1.0);
            m_anim->setCurrentTime(0);
            m_anim->resume();
        }
    }
    else
    {
        if (enter)
        {
            m_anim = new QPropertyAnimation(this, "scale");
            m_anim->setDuration(300);
            m_anim->setKeyValueAt(0, 1.0);
            m_anim->setKeyValueAt(1, 1.3);
            m_anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
        else
        {
            m_anim = new QPropertyAnimation(this, "scale");
            m_anim->setDuration(300);
            m_anim->setKeyValueAt(0, 1.3);
            m_anim->setKeyValueAt(1, 1.0);
            m_anim->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
}

void
GtProcessPropertyPortEntity::updateShape()
{
    if (m_type == INPUT_PORT)
    {
        if (!m_connections.isEmpty())
        {
            setVisible(false);
        }
        else
        {
            setVisible(true);
        }
    }
    else
    {

    }
}

