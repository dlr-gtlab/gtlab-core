/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_graphicsview.cpp
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#include <QtWidgets>

#include "gt_graphicsview.h"
#include "gt_grid.h"
#include "gt_ruler.h"
#include "gt_logging.h"

#include <QGraphicsScene>

struct GtGraphicsView::Impl
{
    /// Options
    Options options = NoOption;

    /// Zoom factor
    double zoom = 1.0;

    /// Max zoom factor
    double maxZoom = 50.0;

    /// Min zoom factor
    double minZoom = 0.05;

    /// Grid
    QPointer<GtGrid> grid;

    /// Horizontal ruler
    QPointer<GtRuler> hRuler;

    /// Vertical ruler
    QPointer<GtRuler> vRuler;

    /// Number of scheduled scalings
    int numsScalings = 0;

    /// Switch for "Snap to Grid" Mode
    bool snap = false;
};

GtGraphicsView::GtGraphicsView(QGraphicsScene* s, Options options, QWidget* parent) :
    QGraphicsView(parent),
    pimpl(std::make_unique<Impl>())
{
    pimpl->options = options;

    setFocusPolicy(Qt::WheelFocus);
    setMouseTracking(true);
    setAutoFillBackground(false);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setScene(s);

    setDragMode(QGraphicsView::ScrollHandDrag);
}

GtGraphicsView::GtGraphicsView(QGraphicsScene* s, QWidget* parent) :
    GtGraphicsView(s, OwnActiveScene, parent)
{ }

GtGraphicsView::~GtGraphicsView()
{
    // delete the scene
    if (pimpl->options.testFlag(OwnActiveScene))
    {
        delete scene();
    }
}

void
GtGraphicsView::setOptions(Options options)
{
    pimpl->options = options;
}

void
GtGraphicsView::setOption(Option option, bool enabled)
{
    pimpl->options.setFlag(option, enabled);
}

GtGraphicsView::Options
GtGraphicsView::options() const
{
    return pimpl->options;
}

GtGrid*
GtGraphicsView::grid()
{
    return pimpl->grid;
}

void
GtGraphicsView::setGrid(GtGrid* grid)
{
    delete pimpl->grid;
    pimpl->grid = grid;

    if (grid)
    {
        grid->setParent(this);
        connect(grid, &GtGrid::visibilityChanged, this, [this](){
            if (viewport()) viewport()->update();
        });
    }
}

void
GtGraphicsView::setHorizontalRuler(GtRuler* ruler)
{
    pimpl->hRuler = ruler;
}

void
GtGraphicsView::setVerticalRuler(GtRuler* ruler)
{
    pimpl->vRuler = ruler;
}

void
GtGraphicsView::wheelEvent(QWheelEvent* e)
{
    if (e->modifiers() & Qt::ControlModifier)
    {
        zoomAnimation(e->angleDelta().y());
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
    QGraphicsView::scrollContentsBy(dx, dy);
}

void
GtGraphicsView::drawBackground(QPainter* painter, const QRectF& rect)
{
    if (!painter) return;

    QGraphicsView::drawBackground(painter, rect);

    if (pimpl->grid)
    {
        pimpl->grid->paint(*painter, rect);
        auto spacing = pimpl->grid->scaledMajorSpacing();
        if (pimpl->vRuler)
        {
            pimpl->vRuler->setGridSpacing(spacing);
            pimpl->vRuler->paint(rect, viewportTransform(), *this);
        }
        if (pimpl->hRuler)
        {
            pimpl->hRuler->setGridSpacing(spacing);
            pimpl->hRuler->paint(rect, viewportTransform(), *this);
        }
    }
}

void
GtGraphicsView::paintEvent(QPaintEvent* event)
{
    QGraphicsView::paintEvent(event);
}

void
GtGraphicsView::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    QGraphicsView::mouseMoveEvent(mouseEvent);

    if (!scene()) return;

    QPointF p = mapToScene(mouseEvent->pos());

    emit mousePositionChanged(p);

    if (pimpl->vRuler) pimpl->vRuler->setCursorPosition(mapFromGlobal(QCursor::pos()));
    if (pimpl->hRuler) pimpl->hRuler->setCursorPosition(mapFromGlobal(QCursor::pos()));

    if (pimpl->snap && (scene()->mouseGrabberItem() &&
                        scene()->selectedItems().size() == 1))
    {
        snapItemToGrid(scene()->mouseGrabberItem(), mouseEvent->pos());
        return;
    }
}

void
GtGraphicsView::setMaximumZoom(double val)
{
    pimpl->maxZoom = val;
}

void
GtGraphicsView::setMinimumZoom(double val)
{
    pimpl->minZoom = val;
}

void
GtGraphicsView::setScale(double val)
{
    pimpl->zoom = pimpl->zoom * val;

    if (pimpl->zoom > pimpl->maxZoom)
    {
        val = val * (pimpl->maxZoom / pimpl->zoom);
        pimpl->zoom = pimpl->maxZoom;
    }
    else if (pimpl->zoom < pimpl->minZoom)
    {
        val = val * (pimpl->minZoom / pimpl->zoom);
        pimpl->zoom = pimpl->minZoom;
    }

    scale(val, val);

    if (pimpl->grid)
    {
        pimpl->grid->setGridScaleFactor(getGridFactor());
    }

    emit zoomChanged(pimpl->zoom);
}

void
GtGraphicsView::setScalePercentage(double percentage)
{
    double factor = (percentage / 100.0f) / pimpl->zoom;
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
    pimpl->numsScalings += steps;

    if (pimpl->numsScalings * steps < 0)
    {
        pimpl->numsScalings = steps;
    }

    QTimeLine* zoomAnimation = new QTimeLine(500, this);

    zoomAnimation->setUpdateInterval(20);

    connect(zoomAnimation, SIGNAL(valueChanged(double)),
            SLOT(scalingTime()));

    connect(zoomAnimation, SIGNAL(finished()), SLOT(animFinished()));

    zoomAnimation->start();
}

int
GtGraphicsView::getGridFactor()
{
    return int(log(1.0 / (pimpl->zoom)) / log(2.0));
}

void
GtGraphicsView::snapToGrid(bool val)
{
    pimpl->snap = val;
}

void
GtGraphicsView::scalingTime()
{
    double factor = 1.0 + double(pimpl->numsScalings) / 300.0;

    setScale(factor);
}

void
GtGraphicsView::animFinished()
{
    if (pimpl->numsScalings > 0)
    {
        pimpl->numsScalings--;
    }
    else
    {
        pimpl->numsScalings++;
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
