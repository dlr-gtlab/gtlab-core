/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineplotcanvas.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SPLINEPLOTCANVAS_H
#define GT_SPLINEPLOTCANVAS_H

#include "qwt_plot_canvas.h"

#include <QPointer>

class GtSplinePlot;
class GtSplinePlotMarker;
class QwtPlotMarker;

/**
 * @brief The GtSplinePlotCanvas class
 */
class GtSplinePlotCanvas  : public QwtPlotCanvas
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GtSplinePlotCanvas(GtSplinePlot* plot);

    /**
     * @brief mouse press event implementation
     * @param event
     */
    virtual void mousePressEvent(QMouseEvent* event);

    /**
     * @brief mouse release event implementation
     * @param event
     */
    virtual void mouseReleaseEvent(QMouseEvent* event);

    /**
     * @brief mouse move event implementation
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event);

private:
    /// Spline plot pointer
    QPointer<GtSplinePlot> m_plot;

    GtSplinePlotMarker* m_selectedMarker;

    /**
     * @brief returns the closest plot marker to a certain position and the
     * related distance
     * @param pos position
     * @param dist distance
     * @return
     */
    GtSplinePlotMarker* closestMarker(const QPoint& pos, double& dist);

};

#endif // GT_SPLINEPLOTCANVAS_H
