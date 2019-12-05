/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineplot.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_panner.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_widget.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_symbol.h"
#include "qwt_plot_rescaler.h"

#include "gt_splineplotcanvas.h"
#include "gt_splinecp.h"
#include "gt_spline.h"
#include "gt_logging.h"
#include "gt_exceptions.h"

#include "gt_command.h"
#include "gt_application.h"
#include "gt_project.h"

#include "sl_common.h"
#include "sl_point.h"
#include "sl_bsplinecurve.h"

#include "gt_splineplot.h"

GtSplinePlotMarker::GtSplinePlotMarker(GtSplineCP* cp) :
    m_cp(cp),
    m_selected(false)
{
    QwtSymbol* sym = new QwtSymbol(QwtSymbol::Rect,
                                   QBrush(Qt::red),
                                   QPen(Qt::black),
                                   QSize(7, 7));

    setSymbol(sym);

    setRenderHint(QwtPlotItem::RenderAntialiased, true);

    if (m_cp != Q_NULLPTR)
    {
        connect(m_cp.data(), SIGNAL(dataChanged(GtObject*)),
                SLOT(onCPChange()));
    }
}

void
GtSplinePlotMarker::setSelected(bool selected)
{
    m_selected = selected;

    if (selected)
    {
        setSymbol(new QwtSymbol(QwtSymbol::Rect,
                                QBrush(Qt::white),
                                QPen(Qt::black),
                                QSize(7, 7)));
    }
    else
    {
        setSymbol(new QwtSymbol(QwtSymbol::Rect,
                                QBrush(Qt::red),
                                QPen(Qt::black),
                                QSize(7, 7)));
    }
}

bool
GtSplinePlotMarker::isSelected()
{
    return m_selected;
}

int
GtSplinePlotMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}

void
GtSplinePlotMarker::setPosition(const QPointF& pos)
{
    //if (m_cp != Q_NULLPTR)
    //{
    //    m_cp->setValue(pos);
    //}

    if (m_cp != Q_NULLPTR)
    {
        QString msg = tr("Control point position changed");
        GtCommand cmd = gtApp->startCommand(gtApp->currentProject(),
                                            msg);

        m_cp->setValue(QPointF(pos.x(), pos.y()));

        gtApp->endCommand(cmd);
    }
}

void
GtSplinePlotMarker::onCPDeleted()
{
    // TBD
}

void
GtSplinePlotMarker::onCPChange()
{
    if (m_cp != Q_NULLPTR)
    {
        setValue(m_cp->x(), m_cp->y());

        emit positionChanged();
    }
}

GtSplinePlot::GtSplinePlot() :
    m_curve(Q_NULLPTR),
    m_spline(Q_NULLPTR),
    m_rawData(nullptr)
{
    setAutoFillBackground(true);
    setPalette(Qt::white);

    m_canvas = new GtSplinePlotCanvas(this);
    setCanvasBackground(Qt::white);
    plotLayout()->setAlignCanvasToScales(true);

    for (int i = 0; i < QwtPlot::axisCnt; ++i)
    {
        QwtScaleWidget* scaleWid = axisWidget(i);

        if (scaleWid != Q_NULLPTR)
        {
            scaleWid->setMargin(0);
        }

        QwtScaleDraw* scaleDraw = axisScaleDraw(i);

        if (scaleDraw != Q_NULLPTR)
        {
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
        }
    }

    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->attach(this);

    QwtPlotPanner* panner = new QwtPlotPanner(m_canvas);
    panner->setMouseButton(Qt::LeftButton, Qt::ControlModifier);
    panner->setAxisEnabled(QwtPlot::yRight, false);

    QwtPlotRescaler* rescaler = new QwtPlotRescaler(m_canvas);
    rescaler->setRescalePolicy(QwtPlotRescaler::Expanding);
    rescaler->setExpandingDirection(QwtPlotRescaler::ExpandBoth);
    rescaler->setAspectRatio(QwtPlot::yRight, 0.0);

    QwtPlotMagnifier* magnifier = new QwtPlotMagnifier(m_canvas);
    magnifier->setWheelModifiers(Qt::ControlModifier);
    magnifier->setAxisEnabled(QwtPlot::yRight, false);
    magnifier->setMouseButton(Qt::NoButton);

    m_picker = new QwtPlotPicker(QwtPlot::xBottom,
                                 QwtPlot::yLeft,
                                 QwtPlotPicker::CrossRubberBand,
                                 QwtPicker::AlwaysOn, m_canvas);
    m_picker->setTrackerPen(Qt::NoPen);
    m_picker->setStateMachine((new QwtPickerClickPointMachine()));
    m_picker->setRubberBandPen(QColor(Qt::black));

    enableAxis(QwtPlot::yLeft, true);
    enableAxis(QwtPlot::xBottom, true);
}

void
GtSplinePlot::initPlot(GtSpline* spline)
{
    if (spline == Q_NULLPTR)
    {
        return;
    }

    setAxisAutoScale(QwtPlot::yLeft, true);
    setAxisAutoScale(QwtPlot::xBottom, true);

    m_spline = spline;

    detachItems(QwtPlotItem::Rtti_PlotCurve);
    detachItems(QwtPlotItem::Rtti_PlotUserItem);
    detachItems(QwtPlotItem::Rtti_PlotMarker);

    m_curve = Q_NULLPTR;

    gtDebug() << "initializing spline plot from datamodel...";

    m_curve = new QwtPlotCurve;
    m_curve->setSymbol(new QwtSymbol(QwtSymbol::NoSymbol));
    m_curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_curve->attach(this);
    m_curve->setPen(Qt::blue, 2.0);

    updateCurve();


    gtDebug() << "initializing raw data from datamodel...";

    m_rawData = new QwtPlotCurve;
    m_rawData->setSymbol(new QwtSymbol(QwtSymbol::Rect));
    m_rawData->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_rawData->attach(this);
    m_rawData->setPen(Qt::blue, 0.0);

    updateRawData();


    QList<GtSplineCP*> cps = spline->controlPoints();

    foreach (GtSplineCP* cp, cps)
    {
        GtSplinePlotMarker* marker = new GtSplinePlotMarker(cp);
        marker->attach(this);
        marker->setValue(cp->x(), cp->y());

        connect(marker, SIGNAL(positionChanged()),
                this, SLOT(onCpDatChanged()));

//        m_tmpMarkerList.append(marker);
    }

//    setAxisAutoScale(QwtPlot::yLeft, false);
//    setAxisAutoScale(QwtPlot::xBottom, false);

    replot();
}

void
GtSplinePlot::selectMarker(GtSplinePlotMarker* marker)
{
    deselectMarkers();

    marker->setSelected(true);

    replot();
}

void
GtSplinePlot::setMarkerPosition(GtSplinePlotMarker* marker, double x, double y)
{
    marker->setPosition(QPointF(x, y));

    updateCurve();
}

void
GtSplinePlot::deselectMarkers()
{
    const QwtPlotItemList& itmList = itemList();

    for (QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it)
    {
        if ((*it)->rtti() == QwtPlotItem::Rtti_PlotUserItem)
        {
            GtSplinePlotMarker* marker =
                static_cast<GtSplinePlotMarker*>(*it);

            marker->setSelected(false);
        }
    }

    replot();
}

void
GtSplinePlot::onCpDatChanged()
{
    updateCurve();

    replot();
}

void
GtSplinePlot::updateCurve()
{
    if (m_curve == Q_NULLPTR)
    {
        return;
    }

    QVector<QPointF> points;

    int pointNumber = 100;

    for (int i = 0; i <= pointNumber; i++)
    {
        double t = (double) i / pointNumber;

        points << curvePoint(t, m_spline);
    }

    m_curve->setSamples(points);
}

void
GtSplinePlot::updateRawData()
{
    if (m_curve == Q_NULLPTR)
    {
        return;
    }

    QVector<QPointF> points;

    for (int i = 0; i < m_spline->rawPointsX().size(); ++i)
    {
        points << QPointF(m_spline->rawPointsX().at(i),
                          m_spline->rawPointsY().at(i));
    }

    m_rawData->setSamples(points);
}

QPointF
GtSplinePlot::curvePoint(double t, GtSpline* spline)
{
    if (spline == Q_NULLPTR)
    {
        return QPointF();
    }

    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    SLPoint p_temp = spline_tmp.eval2(t);

    return QPointF(p_temp.X(), p_temp.Y());

//    QVector<double> cp_x;
//    QVector<double> cp_y;

//    foreach (const QPointF& cp, spline->controlPointsConverted())
//    {
//        cp_x.append(cp.x());
//        cp_y.append(cp.y());
//    }

//    double x = 0.0;
//    double y = 0.0;

//    QVector<double> knot = spline->KnotVec();

//    try
//    {
//        x = GtBSplineAlgorithm::BCurveValue(knot,
//                                            cp_x, spline->degree() + 1, 0, t);
//    }
//    catch (GTlabException &e)
//    {
//        gtError() << "Exception in function" << e.where() << "!";
//        gtError() << "Explanation:" << e.what();
//        return QPointF();
//    }

//    try
//    {
//        y = GtBSplineAlgorithm::BCurveValue(knot, cp_y,
//                                            spline->degree() + 1, 0, t);
//    }
//    catch (GTlabException &e)
//    {
//        gtError() << "Exception in function" << e.where() << "!";
//        gtError() << "Explanation:" << e.what();
//        return QPointF();
//    }

//    return QPointF(x, y);
}
