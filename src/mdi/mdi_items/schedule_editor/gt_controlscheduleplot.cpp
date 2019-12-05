/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlscheduleplot.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 23.02.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_controlscheduleplot.h"

#include <QPointF>
#include <QMessageBox>

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

#include "gt_controlschedule.h"
#include "gt_controlschedulecanvas.h"

GtControlSchedulePlot::GtControlSchedulePlot() :
    m_linCurve(Q_NULLPTR), m_origCurve(Q_NULLPTR),
    m_editModeActive(false), m_canvas(Q_NULLPTR)
{
    setAutoFillBackground(true);
    setPalette(Qt::white);

    m_canvas = new GtControlScheduleCanvas(this);
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

    setAxisAutoScale(QwtPlot::yLeft, false);
    setAxisAutoScale(QwtPlot::xBottom, false);

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
GtControlSchedulePlot::initPlotFromSchedule(GtControlSchedule* schedule)
{
    if (!isValidSchedule(schedule))
    {
        return;
    }

    setAxisAutoScale(QwtPlot::yLeft, true);
    setAxisAutoScale(QwtPlot::xBottom, true);

    m_schedule = schedule;

    detachItems(QwtPlotItem::Rtti_PlotCurve);
    detachItems(QwtPlotItem::Rtti_PlotUserItem);
    detachItems(QwtPlotItem::Rtti_PlotMarker);

    m_linCurve = Q_NULLPTR;
    m_origCurve = Q_NULLPTR;

    m_tmpMarkerList.clear();
    m_origMarkerList.clear();

    QVector<double> xaxis = schedule->getAxisTicks(0);

    QString xAxisTitle = schedule->getAxesNames().first();
    QString yAxisTitle = schedule->valueNames().first();

    QList<QString> keys = schedule->tabValsKeys();

    QVector<double> vals = schedule->tabVals(keys.first());

    setXtitle(xAxisTitle);
    setYtitle(yAxisTitle);

    setXunit(schedule->getAxesList().first()->unit());
    setXdescription(schedule->getAxesList().first()->description());
    setYunit(schedule->valUnit(yAxisTitle));
    setYdescription(schedule->valDescription(yAxisTitle));

    QVector<QPointF> points;

    for (int i = 0; i < xaxis.size(); i++)
    {
        points.append(QPointF(xaxis.at(i), vals.at(i)));
    }

    foreach (QPointF point, points)
    {
        GtControlScheduleMarker* marker = new GtControlScheduleMarker;
        marker->attach(this);
        marker->setValue(point);
        m_tmpMarkerList.append(marker);
    }

    updateOrigCurve();

    if (editModeActive())
    {
        updateLinCurve();
    }

    setAxisAutoScale(QwtPlot::yLeft, false);
    setAxisAutoScale(QwtPlot::xBottom, false);

    emit pointAddedOrRemoved();
}

void
GtControlSchedulePlot::deselectMarkers()
{
    if (!hasSelectedMarkers())
    {
        return;
    }

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        marker->setSelected(false);
    }

    replot();

    emit markersDeselected();
}

void
GtControlSchedulePlot::selectMarker(GtControlScheduleMarker* marker)
{
    deselectMarkers();
    if (m_tmpMarkerList.contains(marker))
    {
        marker->setSelected();
        replot();

        emit markerSelected(marker);
    }
}

void
GtControlSchedulePlot::setMarkerPosition(GtControlScheduleMarker* marker,
        double x, double y)
{
    if (m_tmpMarkerList.contains(marker))
    {
        marker->setValue(QPointF(x, y));
        sortMarkers();
        updateLinCurve();
        emit pointAddedOrRemoved();
    }
}

void
GtControlSchedulePlot::deleteMarker(GtControlScheduleMarker* marker)
{
    if (!m_tmpMarkerList.contains(marker))
    {
        return;
    }

    deselectMarkers();

    if (m_tmpMarkerList.size() <= 2)
    {
        QMessageBox msgBox;
        msgBox.setText("Minimum number of points for a schedule is 2 \n"
                       "|--> No further point deletion allowed!");
        msgBox.exec();
        return;
    }

    m_tmpMarkerList.removeAll(marker);
    marker->detach();
    delete marker;

    updateLinCurve();

    emit pointAddedOrRemoved();
}

void
GtControlSchedulePlot::activateEditMode(bool active)
{
    m_editModeActive = active;
    plotCanvas()->activateAddPointState(false);

    if (active)
    {
        if (m_origCurve != Q_NULLPTR)
        {
            m_origCurve->setPen(Qt::gray, 2.0);
        }

        updateLinCurve();
    }
    else
    {
        if (m_linCurve != Q_NULLPTR)
        {
            m_linCurve->detach();
            delete m_linCurve;
            m_linCurve = Q_NULLPTR;

            updateOrigCurve();
        }
    }

    emit editModeChanged(active);
}

bool
GtControlSchedulePlot::editModeActive()
{
    return m_editModeActive;
}

GtControlScheduleCanvas*
GtControlSchedulePlot::plotCanvas()
{
    return m_canvas;
}

void
GtControlSchedulePlot::addPoint(double x, double y)
{
    GtControlScheduleMarker* marker = new GtControlScheduleMarker;
    m_tmpMarkerList.append(marker);
    marker->attach(this);
    setMarkerPosition(marker, x, y);
}

QList<QPointF>
GtControlSchedulePlot::dataPoints()
{
    QList<QPointF> points;

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        points.append(marker->value());
    }

    return points;
}

QString
GtControlSchedulePlot::xTitle()
{
    return m_xTitle;
}

QString
GtControlSchedulePlot::yTitle()
{
    return m_yTitle;
}

void
GtControlSchedulePlot::setXtitle(QString title)
{
    m_xTitle = title;

    QString titleAndUnit = title + " [" + xUnit() + "]";
    setAxisTitle(QwtPlot::xBottom, titleAndUnit);
}

void
GtControlSchedulePlot::setYtitle(QString title)
{
    m_yTitle = title;

    QString titleAndUnit = title + " [" + yUnit() + "]";
    setAxisTitle(QwtPlot::yLeft, titleAndUnit);
}

void
GtControlSchedulePlot::setXdescription(QString descr)
{
    m_xDescr = descr;
}

void
GtControlSchedulePlot::setYdescription(QString descr)
{
    m_yDescr = descr;
}

void
GtControlSchedulePlot::setXunit(QString unit)
{
    m_xUnit = unit;
    setXtitle(xTitle());
}

void
GtControlSchedulePlot::setYunit(QString unit)
{
    m_yUnit = unit;
    setYtitle(yTitle());
}

QString
GtControlSchedulePlot::xUnit()
{
    return m_xUnit;
}

QString
GtControlSchedulePlot::yUnit()
{
    return m_yUnit;
}

QString
GtControlSchedulePlot::xDescription()
{
    return m_xDescr;
}

QString
GtControlSchedulePlot::yDescription()
{
    return m_yDescr;
}

int
GtControlSchedulePlot::nPoints()
{
    return m_tmpMarkerList.size();
}

QVector<double>
GtControlSchedulePlot::xValues()
{
    QVector<double> retVal;

    foreach (QPointF point, dataPoints())
    {
        retVal.append(point.x());
    }

    return retVal;
}

QVector<double>
GtControlSchedulePlot::yValues()
{
    QVector<double> retVal;

    foreach (QPointF point, dataPoints())
    {
        retVal.append(point.y());
    }

    return retVal;
}

void
GtControlSchedulePlot::switchToClickPointMachine()
{
    m_picker->setTrackerPen(Qt::NoPen);
    m_picker->setStateMachine(new QwtPickerClickPointMachine());
}

void
GtControlSchedulePlot::switchToClickPointTrackerMachine()
{
    m_picker->setTrackerPen(QColor(Qt::black));
    m_picker->setStateMachine(new QwtPickerClickPointMachine());
}

void
GtControlSchedulePlot::switchToPickerTrackerMachine()
{
    m_picker->setTrackerPen(QColor(Qt::black));
    m_picker->setStateMachine(new QwtPickerTrackerMachine());
}

GtControlSchedule*
GtControlSchedulePlot::schedule()
{
    return m_schedule;
}

QList<GtControlScheduleMarker*>
GtControlSchedulePlot::tmpMarkerList()
{
    return m_tmpMarkerList;
}

bool
GtControlSchedulePlot::hasSelectedMarkers()
{
    foreach(GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        if (marker->isSelected())
        {
            return true;
        }
    }

    return false;
}

bool
GtControlSchedulePlot::isValidSchedule(GtControlSchedule* schedule)
{
    if (schedule == 0)
    {
        gtWarning() << tr("Schedule is a Q_NULLPTR");
        return false;
    }

    QList<QString> axesNames = schedule->getAxesNames();

    if (axesNames.size() == 0)
    {
        gtWarning() << tr("There are no axis in this table");
        return false;
    }

    if (axesNames.size() != 1)
    {
        gtWarning() << tr("There are too many axis in this table");
        gtWarning() << tr("Defined Axis: ") << axesNames.size();
        gtWarning() << tr("Allowed: 1");
        return false;
    }

    QList<QString> keys = schedule->tabValsKeys();

    if (keys.size() == 0)
    {
        gtWarning() << tr("There are no keys in this table");
        return false;
    }

    if (keys.size() != 1)
    {
        gtWarning() << tr("There are too many keys defined in this table");
        gtWarning() << tr("Defined Keys: ") << keys.size();
        gtWarning() << tr("Allowed: 1");
        return false;
    }

    return true;
}

void
GtControlSchedulePlot::updateLinCurve()
{
    if (m_linCurve == Q_NULLPTR)
    {
        m_linCurve = new QwtPlotCurve;
        m_linCurve->setSymbol(new QwtSymbol(QwtSymbol::NoSymbol));
        m_linCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        m_linCurve->setPen(Qt::red, 1.0);
        m_linCurve->attach(this);
    }

    QVector<QPointF> points;

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        points.append(marker->value());
    }

    m_linCurve->setSamples(points);

    replot();
}

void
GtControlSchedulePlot::updateOrigCurve()
{
    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    if (m_origCurve == Q_NULLPTR)
    {
        m_origCurve = new QwtPlotCurve;
        m_origCurve->setSymbol(new QwtSymbol(QwtSymbol::NoSymbol));
        m_origCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        m_origCurve->attach(this);
    }

    if (!editModeActive())
    {
        m_origCurve->setPen(Qt::blue, 2.0);
    }
    else
    {
        m_origCurve->setPen(Qt::gray, 2.0);
    }

    m_origCurve->setSamples(pointsFromSchedule());

    foreach (QwtPlotMarker* marker, m_origMarkerList)
    {
        marker->detach();
        delete marker;
    }

    m_origMarkerList.clear();

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        QwtPlotMarker* origMarker = new QwtPlotMarker;
        origMarker->setRenderHint(QwtPlotItem::RenderAntialiased, true);
        origMarker->setSymbol(new QwtSymbol(QwtSymbol::Triangle,
                                            QBrush(Qt::darkGray),
                                            QPen(Qt::darkGray),
                                            QSize(7, 7)));

        origMarker->setValue(marker->value());
        origMarker->attach(this);
        m_origMarkerList.append(origMarker);
    }

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        marker->detach();
        marker->attach(this);
    }

    replot();
}

void
GtControlSchedulePlot::sortMarkers()
{
    QList<GtControlScheduleMarker*> tmpMarkers;

    foreach (GtControlScheduleMarker* marker, m_tmpMarkerList)
    {
        tmpMarkers.append(marker);
    }

    m_tmpMarkerList.clear();

    m_tmpMarkerList.append(tmpMarkers.first());
    tmpMarkers.removeAll(m_tmpMarkerList.first());

    foreach (GtControlScheduleMarker* marker, tmpMarkers)
    {
        QList<GtControlScheduleMarker*>::iterator i;

        for (i = m_tmpMarkerList.begin(); i != m_tmpMarkerList.end(); ++i)
        {
            GtControlScheduleMarker* m = *i;

            if (marker->value().x() < m->value().x())
            {
                m_tmpMarkerList.insert(i, marker);
                break;
            }
        }

        if (!m_tmpMarkerList.contains(marker))
        {
            m_tmpMarkerList.append(marker);
        }
    }
}

QVector<QPointF>
GtControlSchedulePlot::pointsFromSchedule()
{
    QVector<QPointF> retVal;

    if (m_schedule == Q_NULLPTR)
    {
        return retVal;
    }

    QVector<double> ticks = m_schedule->getAxisTicks(0);

    double x = 0;
    double y = 0;

    if (m_schedule->getAxesList().first()->interMethod()
            == GtTableAxis::I_LINEAR)
    {
        foreach (double val, ticks)
        {
            try
            {
                x = val;
                y = m_schedule->getValue1D(m_schedule->valueNames().first(), x);
                retVal.append(QPointF(x, y));
            }
            catch(GTlabException& e)
            {
                gtError() << "GTlabException";
                gtError() << "\tWhere: "
                             "GtControlSchedulePlot::pointsFromSchedule() -> "
                          << e.where();
                gtError() << "\tWhat: " << e.what();
                return retVal;
            }
        }

        return retVal;
    }

    double min = ticks.first();
    double max = ticks.last();

    int nPoints = 150;

    for (int i = 0; i < nPoints; i++)
    {
        x = min + (max - min) / (nPoints - 1.0) * i;
        y = m_schedule->getValue1D(m_schedule->valueNames().first(), x);

        retVal.append(QPointF(x, y));
    }

    return retVal;
}

GtControlScheduleMarker::GtControlScheduleMarker() :
    m_selected(false)
{
    QwtSymbol* sym = new QwtSymbol(QwtSymbol::Ellipse,
                                   QBrush(Qt::black),
                                   QPen(Qt::black),
                                   QSize(7, 7));

    setSymbol(sym);

    setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

void
GtControlScheduleMarker::setSelected(bool selected)
{
    m_selected = selected;

    if (selected)
    {
        setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                QBrush(Qt::white),
                                QPen(Qt::black),
                                QSize(7, 7)));
    }
    else
    {
        setSymbol(new QwtSymbol(QwtSymbol::Ellipse,
                                QBrush(Qt::black),
                                QPen(Qt::black),
                                QSize(7, 7)));
    }
}

bool
GtControlScheduleMarker::isSelected()
{
    return m_selected;
}

int
GtControlScheduleMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}
