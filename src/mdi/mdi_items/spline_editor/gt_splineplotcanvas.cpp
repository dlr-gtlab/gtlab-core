/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineplotcanvas.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMouseEvent>

#include "gt_splineplot.h"

#include <qmath.h>

#include "gt_splineplotcanvas.h"

GtSplinePlotCanvas::GtSplinePlotCanvas(GtSplinePlot* plot) :
    m_plot(plot),
    m_selectedMarker(Q_NULLPTR)
{
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(2);
    setCursor(Qt::ArrowCursor);

    if (m_plot != Q_NULLPTR)
    {
        m_plot->setCanvas(this);
    }
}

void
GtSplinePlotCanvas::mousePressEvent(QMouseEvent* event)
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    double dist = 10e10;

    GtSplinePlotMarker* marker = closestMarker(event->pos(), dist);

    if (dist <= 3)
    {
        if (marker != Q_NULLPTR)
        {
            m_plot->selectMarker(marker);
//            m_plot->switchToClickPointTrackerMachine();
            m_selectedMarker = marker;
//            m_readyToMove = true;
        }
    }
    else
    {
        m_plot->deselectMarkers();
//        if (m_plot->hasSelectedMarkers())
//        {
//            m_plot->deselectMarkers();
//        }

        m_selectedMarker = Q_NULLPTR;
    }

    QwtPlotCanvas::mousePressEvent(event);
}

void
GtSplinePlotCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    m_plot->deselectMarkers();
    m_selectedMarker = Q_NULLPTR;

    QwtPlotCanvas::mousePressEvent(event);
}

void
GtSplinePlotCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
    const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

    double xCoord = xMap.invTransform(event->pos().x());
    double yCoord = yMap.invTransform(event->pos().y());

    if (m_selectedMarker != Q_NULLPTR)
    {
        m_plot->setMarkerPosition(m_selectedMarker, xCoord, yCoord);
    }

    m_plot->replot();

    QwtPlotCanvas::mousePressEvent(event);
}

GtSplinePlotMarker*
GtSplinePlotCanvas::closestMarker(const QPoint& pos, double& dist)
{
    if (m_plot == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtSplinePlotMarker* retVal = Q_NULLPTR;

    const QwtPlotItemList& itmList = m_plot->itemList();

    for (QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it)
    {
        if ((*it)->rtti() == QwtPlotItem::Rtti_PlotUserItem)
        {
            GtSplinePlotMarker* marker =
                static_cast<GtSplinePlotMarker*>(*it);

            if (marker != Q_NULLPTR)
            {
                QPointF val = marker->value();

                const QwtScaleMap xMap = m_plot->canvasMap(QwtPlot::xBottom);
                const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

                const double cx = xMap.transform(val.x()) - pos.x();
                const double cy = yMap.transform(val.y()) - pos.y();

                const double f = qSqrt(qAbs(cx)) + qSqrt(qAbs(cy));

                if (f < dist)
                {
                    dist = f;
                    retVal = marker;
                }
            }
        }
    }

    return retVal;
}
