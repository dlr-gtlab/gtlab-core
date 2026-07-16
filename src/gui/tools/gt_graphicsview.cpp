/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_graphicsview.cpp
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#include "gt_graphicsview.h"
#include "gt_grid.h"
#include "gt_ruler.h"

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPointer>
#include <QTimeLine>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>

#include <cmath>

struct GtGraphicsView::Impl
{
    /// Options
    Options options = NoOption;

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

    double snapThreshold = 10.0;

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

GtGraphicsView::GtGraphicsView(Options options, QWidget* parent) :
    GtGraphicsView(nullptr, options, parent)
{ }

GtGraphicsView::~GtGraphicsView()
{
    // delete the scene
    if (pimpl->options.testFlag(DestroyActiveSceneOnDeletion))
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
        connect(grid, &GtGrid::updated, this, [this](){
            if (viewport()) viewport()->update();
        });
    }
}

void
GtGraphicsView::connectHorizontalRuler(GtRuler* ruler)
{
    pimpl->hRuler = ruler;
    if (ruler) ruler->setDrawOffset(frameShape() == QFrame::NoFrame ? 0 : frameWidth());
}

void
GtGraphicsView::connectVerticalRuler(GtRuler* ruler)
{
    pimpl->vRuler = ruler;
    if (ruler) ruler->setDrawOffset(frameShape() == QFrame::NoFrame ? 0 : frameWidth());
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
        auto spacing = pimpl->grid->currentGridSpacing();

        if (pimpl->vRuler)
        {
            pimpl->vRuler->paint(spacing, rect, viewportTransform());
        }
        if (pimpl->hRuler)
        {
            pimpl->hRuler->paint(spacing, rect, viewportTransform());
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

    QPoint cursorPos = mouseEvent->pos();
    if (pimpl->vRuler) pimpl->vRuler->setCursorPosition(cursorPos);
    if (pimpl->hRuler) pimpl->hRuler->setCursorPosition(cursorPos);

    if (!scene()) return;

    QPointF scenePos = mapToScene(cursorPos);

    emit mousePositionChanged(scenePos);

    if (pimpl->snap && (scene()->mouseGrabberItem() &&
                        scene()->selectedItems().size() == 1))
    {
        snapItemToGrid(scene()->mouseGrabberItem(), mouseEvent->pos());
        return;
    }
}

void
GtGraphicsView::setZoomPercentage(double percentage)
{
    double factor = (percentage / 100.0) / zoom();
    setScale(factor);
}

double
GtGraphicsView::zoomPercentage() const
{
    return zoom() * 100.0;
}

double
GtGraphicsView::zoom() const
{
    return transform().m11();
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

bool
GtGraphicsView::snapToGrid() const
{
    return pimpl->snap;
}

bool
GtGraphicsView::snapToGridThreshold() const
{
    return pimpl->snapThreshold;
}

void
GtGraphicsView::setScale(double scale)
{
    double zoom = this->zoom() * scale;

    if (zoom > pimpl->maxZoom)
    {
        scale = scale * (pimpl->maxZoom / zoom);
        zoom = pimpl->maxZoom;
    }
    else if (zoom < pimpl->minZoom)
    {
        scale = scale * (pimpl->minZoom / zoom);
        zoom = pimpl->minZoom;
    }

    this->scale(scale, scale);

    emit zoomChanged(zoom);
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

void
GtGraphicsView::setSnapToGrid(bool enable)
{
    pimpl->snap = enable;
}

void
GtGraphicsView::setSnapToGridThreshold(double threshold)
{
    pimpl->snapThreshold = std::max(0.0, threshold);
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

    QPointF scenePos = mapToScene(mousePos);

    QPointF np = grid()->computeNearestGridPoint(scenePos);

    QLineF line{mousePos, mapFromScene(np)};

    double length = line.length();

    if (pimpl->snapThreshold <= 0 || length < pimpl->snapThreshold)
    {
        item->setPos(np - ibrc);
    }
    else
    {
        item->setPos(scenePos - ibrc);
    }
}

void
GtGraphicsView::snapItemToGrid(QGraphicsItem* item)
{
    QPointF ibrc = item->boundingRect().center();
    QPointF np = grid()->computeNearestGridPoint(item->mapToScene(ibrc));
    item->setPos(np - ibrc);
}
