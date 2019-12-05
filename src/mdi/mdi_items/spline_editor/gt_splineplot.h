/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineplot.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SPLINEPLOT_H
#define GT_SPLINEPLOT_H

#include <QPointer>

#include "qwt_plot.h"
#include "qwt_plot_marker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_picker.h"


class QPointF;
class QwtPlotCurve;
class QwtPlotPicker;
class GtSplinePlotCanvas;
class GtSpline;
class GtSplineCP;

/**
 * @brief The GtSplinePlotMarker class
 */
class GtSplinePlotMarker : public QObject, public QwtPlotMarker
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    GtSplinePlotMarker(GtSplineCP* cp);

    /**
     * @brief set marker state to selected
     */
    void setSelected(bool selected = true);

    /**
     * @brief returns true in case that marker is currently selected
     * @return
     */
    bool isSelected();

    /**
     * @brief returns unique plot item identification number
     * @return
     */
    virtual int rtti() const;

    /**
     * @brief setPosition
     * @param pos
     */
    void setPosition(const QPointF& pos);

private:
    /// Control point datamodel object
    QPointer<GtSplineCP> m_cp;

    /// selected state
    bool m_selected;

private slots:
    /**
     * @brief onCPDeleted
     */
    void onCPDeleted();

    /**
     * @brief onCPChange
     */
    void onCPChange();
signals:
    void positionChanged();
};

/**
 * @brief The GtSplinePlot class
 */
class GtSplinePlot : public QwtPlot
{
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     */
    GtSplinePlot();

    /**
     * @brief initPlot
     * @param spline
     */
    void initPlot(GtSpline* spline);

    /**
     * @brief select a certain marker
     * @param marker
     */
    void selectMarker(GtSplinePlotMarker* marker);

    /**
     * @brief set position of a certain marker
     * @param marker
     * @param x
     * @param y
     */
    void setMarkerPosition(GtSplinePlotMarker* marker, double x, double y);

    /**
     * @brief deselect all markers
     */
    void deselectMarkers();
public slots:
    void onCpDatChanged();
private:
    /// custom plot canvas
    GtSplinePlotCanvas* m_canvas;

    /// plot picker
    QwtPlotPicker* m_picker;

    /// spline curve
    QwtPlotCurve* m_curve;

    /// Optional raw data
    QwtPlotCurve* m_rawData;

    /// Spline pointer
    QPointer<GtSpline> m_spline;

    /**
     * @brief updateCurve
     */
    void updateCurve();

    /**
     * @brief updateRawData - update plot of raw data points
     */
    void updateRawData();

    /**
     * @brief curvePoint
     * @param t
     * @param spline
     * @return
     */
    QPointF curvePoint(double t, GtSpline* spline);

};

#endif // GT_SPLINEPLOT_H
