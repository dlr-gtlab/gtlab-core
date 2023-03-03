/* GTlab - Gas Turbine laboratory
 * Source File: gtd_graphicsview.cpp
 * copyright 2009-2013 by DLR
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#include <QtWidgets>

#include "gt_graphicsview.h"
#include "gt_graphicsscene.h"
#include "gt_grid.h"
#include "gt_ruler.h"
#include "gt_logging.h"

//#include "gtd_settings.h"

GtGraphicsView::GtGraphicsView(GtGraphicsScene *s, QWidget *parent) :
    QGraphicsView(parent),
    m_scene(s),
    m_zoom(1.0),
    m_maxZoom(50.0),
    m_minZoom(0.05),
    m_numsScalings(0),
    m_grid(nullptr),
    m_hRuler(nullptr),
    m_vRuler(nullptr),
    m_snap(false)
{
    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setAutoFillBackground(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setScene(m_scene);

    setDragMode(QGraphicsView::ScrollHandDrag);

//    if(GtdAppearanceSettings().useAntiAliasing())
//    {
//        setRenderHints(QPainter::Antialiasing |
//                       QPainter::SmoothPixmapTransform);
//    }
}

GtGraphicsView::~GtGraphicsView()
{
    // delete the scene
    delete m_scene;

    // delte grid
    delete m_grid;
}

GtGrid*
GtGraphicsView::grid()
{
    return m_grid;
}

void
GtGraphicsView::setGrid(GtGrid* grid)
{
    delete m_grid;

    m_grid = grid;

    connect(grid, SIGNAL(update()), viewport(), SLOT(update()));
}

void
GtGraphicsView::setHorizontalRuler(GtRuler *ruler)
{
    if (!m_grid)
    {
        gtWarning().verbose().nospace()
                << __FUNCTION__ << ": " << tr("Could not set Ruler. Set grid first!");
        return;
    }

    m_hRuler = ruler;
}

void
GtGraphicsView::setVerticalRuler(GtRuler *ruler)
{
    if (!m_grid)
    {
        gtWarning().verbose().nospace()
                << __FUNCTION__ << ": " << tr("Could not set Ruler. Set grid first!");
        return;
    }

    m_vRuler = ruler;
}

void
GtGraphicsView::wheelEvent(QWheelEvent* e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        zoomAnimation(e->delta());
        e->accept();
    }
    else
    {
        QGraphicsView::wheelEvent(e);
    }
}

void
GtGraphicsView::scrollContentsBy(int dx, int dy)
{
    if (m_grid)
    {
        if (m_hRuler)
        {
            m_hRuler->setNeedsRepaint(true);
        }

        if (m_vRuler)
        {
            m_vRuler->setNeedsRepaint(true);
        }
    }

    QGraphicsView::scrollContentsBy(dx, dy);

}

void
GtGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawBackground(painter, rect);

    if (m_grid)
    {
        if (rect.isValid())
        {
            m_grid->paintGrid(painter, rect);
        }
        else
        {
            gtWarning() << tr("Model contains invalid values "
                              "for geometrical description");
        }
    }
}

void
GtGraphicsView::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    QGraphicsView::mouseMoveEvent(mouseEvent);

    QPointF p = mapToScene(mouseEvent->pos());

    if (m_hRuler)
    {
        m_hRuler->setCursorPosition(mouseEvent->pos());
    }

    if (m_vRuler)
    {
        m_vRuler->setCursorPosition(mouseEvent->pos());
    }

    emit mousePositionChanged(p);

    if (m_snap && (scene()->selectedItems().size() == 1) &&
            scene()->mouseGrabberItem())
    {
        snapItemToGrid(scene()->mouseGrabberItem(), mouseEvent->pos());
        return;
    }   
}

void
GtGraphicsView::setMaximumZoom(qreal val)
{
    m_maxZoom = val;
}

void
GtGraphicsView::setMinimumZoom(qreal val)
{
    m_minZoom = val;
}

void
GtGraphicsView::setScale(qreal val)
{
    m_zoom = m_zoom * val;

    if (m_zoom > m_maxZoom)
    {
        val = val * (m_maxZoom / m_zoom);
        m_zoom = m_maxZoom;
    }
    else if (m_zoom < m_minZoom)
    {
        val = val * (m_minZoom / m_zoom);
        m_zoom = m_minZoom;
    }

    scale(val, val);

    if (m_grid)
    {
        m_grid->setGridScaleFactor(getGridFactor());
    }

    emit zoomChanged(m_zoom);
}

void
GtGraphicsView::setScalePercentage(qreal percentage)
{
    qreal factor = (percentage / 100.0f) / m_zoom;
    setScale(factor);
}

void
GtGraphicsView::zoomAnimation(int delta)
{
    if (delta > 0)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
    else
    {
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    }

    int degrees = delta / 8;
    int steps = degrees / 15;
    m_numsScalings += steps;

    if (m_numsScalings * steps < 0)
    {
        m_numsScalings = steps;
    }

    QTimeLine* zoomAnimation = new QTimeLine(500, this);

    zoomAnimation->setUpdateInterval(20);

    connect(zoomAnimation, SIGNAL(valueChanged(qreal)),
            SLOT(scalingTime()));

    connect(zoomAnimation, SIGNAL(finished()), SLOT(animFinished()));

    zoomAnimation->start();
}

int
GtGraphicsView::getGridFactor()
{
    return int(log(1.0 / (m_zoom)) / log(2.0));
}

void
GtGraphicsView::repaintRuler()
{
    if (m_grid)
    {
        if (m_hRuler)
        {
            m_grid->paintRuler(m_hRuler);
            m_hRuler->repaint();
        }

        if(m_vRuler != nullptr)
        {
            m_grid->paintRuler(m_vRuler);
            m_vRuler->repaint();
        }
    }
}

void
GtGraphicsView::snapToGrid(bool val)
{
    m_snap = val;
}

void
GtGraphicsView::scalingTime()
{
    qreal factor = 1.0 + qreal(m_numsScalings) / 300.0;

    setScale(factor);
}

void
GtGraphicsView::animFinished()
{
    if (m_numsScalings > 0)
    {
        m_numsScalings--;
    }
    else
    {
        m_numsScalings++;
    }

    sender()->~QObject();
}

void
GtGraphicsView::snapItemToGrid(QGraphicsItem* item, QPoint mousePos)
{
    if ( !(item->flags() & QGraphicsItem::ItemIsMovable))
    {
        return;
    }

    QPointF ibrc = item->boundingRect().center();

    QPointF np = grid()->computeNearestGridPoint(mapToScene(mousePos));

    QLineF line(mousePos, mapFromScene(np));

    double length = line.length();

    if (length < 10)
    {
        item->setPos(np - ibrc);
    }
    else
    {
        item->setPos(mapToScene(mousePos) - ibrc);
    }
}

void
GtGraphicsView::snapItemToGrid(QGraphicsItem* item)
{
    QPointF ibrc = item->boundingRect().center();
    QPointF np = grid()->computeNearestGridPoint(item->mapToScene(ibrc));
    item->setPos(np - ibrc);
}
