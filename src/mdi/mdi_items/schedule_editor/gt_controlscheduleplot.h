/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlscheduleplot.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 23.02.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTROLSCHEDULEPLOT_H
#define GTCONTROLSCHEDULEPLOT_H

#include "qwt_plot.h"
#include "qwt_plot_marker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_picker.h"


class GtControlSchedule;
class QPointF;
class QwtPlotCurve;
class GtControlScheduleCanvas;
class QwtPlotPicker;

/**
 * @brief The GtControlScheduleMarker class
 */
class GtControlScheduleMarker : public QwtPlotMarker
{
public:
    /**
     * @brief Constructor
     */
    GtControlScheduleMarker();

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

private:
    /// selected state
    bool m_selected;
};

/**
 * @brief The GtControlSchedulePlot class
 */
class GtControlSchedulePlot : public QwtPlot
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    GtControlSchedulePlot();

    /**
     * @brief plot initialisation from schedule object
     * @param schedule
     */
    void initPlotFromSchedule(GtControlSchedule* schedule);

    /**
     * @brief deselect all markers
     */
    void deselectMarkers();

    /**
     * @brief select a certain marker
     * @param marker
     */
    void selectMarker(GtControlScheduleMarker* marker);

    /**
     * @brief set position of a certain marker
     * @param marker
     * @param x
     * @param y
     */
    void setMarkerPosition(GtControlScheduleMarker* marker, double x, double y);

    /**
     * @brief delete a certain marker (point) in the schedule
     * @param marker
     */
    void deleteMarker(GtControlScheduleMarker* marker);

    /**
     * @brief activate plot editing
     * @param active
     */
    void activateEditMode(bool active = true);

    /**
     * @brief return current edit mode state
     * @return
     */
    bool editModeActive();

    /**
     * @brief return point picker member
     * @return
     */
    GtControlScheduleCanvas* plotCanvas();

    /**
     * @brief add point to schedule
     * @param x
     * @param y
     */
    void addPoint(double x, double y);

    /**
     * @brief points included in schedule
     * @return
     */
    QList<QPointF> dataPoints();

    /**
     * @brief x axis title
     * @return
     */
    QString xTitle();

    /**
     * @brief y axis title
     * @return
     */
    QString yTitle();

    /**
     * @brief setXtitle
     */
    void setXtitle(QString title);

    /**
     * @brief setYtitle
     */
    void setYtitle(QString title);

    /**
     * @brief setXdescription
     * @param descr
     */
    void setXdescription(QString descr);

    /**
     * @brief setYdescription
     * @param descr
     */
    void setYdescription(QString descr);

    /**
     * @brief setXunit
     * @param unit
     */
    void setXunit(QString unit);

    /**
     * @brief setYunit
     * @param unit
     */
    void setYunit(QString unit);

    /**
     * @brief xUnit
     * @return
     */
    QString xUnit();

    /**
     * @brief yUnit
     * @return
     */
    QString yUnit();

    /**
     * @brief xDescription
     * @return
     */
    QString xDescription();

    /**
     * @brief yDescription
     * @return
     */
    QString yDescription();

    /**
     * @brief current number of points
     * @return
     */
    int nPoints();

    /**
     * @brief values on x axis
     * @return
     */
    QVector<double> xValues();

    /**
     * @brief values on y axis
     * @return
     */
    QVector<double> yValues();

    /**
     * @brief switches plot picker machine to click point machine
     */
    void switchToClickPointMachine();

    /**
     * @brief switches plot picker machine to point tracker machine
     */
    void switchToClickPointTrackerMachine();

    /**
     * @brief switches plot picker machine to click point machine with
     * marker text
     */
    void switchToPickerTrackerMachine();

    /**
     * @brief returns the schedule on which the plot is based
     * @return
     */
    GtControlSchedule* schedule();

    /**
     * @brief tmpMarkerList
     * @return
     */
    QList<GtControlScheduleMarker*> tmpMarkerList();

    /**
     * @brief hasSelectedMarkers
     * @return
     */
    bool hasSelectedMarkers();

private:
    /**
     * @brief check schedule on validity
     * @param schedule
     * @return
     */
    bool isValidSchedule(GtControlSchedule* schedule);

    /**
     * @brief update the curve connecting the points with simple linear lines
     */
    void updateLinCurve();

    /**
     * @brief update the curve including the current interpolation method
     */
    void updateOrigCurve();

    /**
     * @brief sort markers with regard to their x position
     */
    void sortMarkers();

    /**
     * @brief returns the points currently included in the schedule
     * @return
     */
    QVector<QPointF> pointsFromSchedule();

    /// List of temporary plot markers
    QList<GtControlScheduleMarker*> m_tmpMarkerList;

    /// List of plot markers based on the original schedule
    QList<QwtPlotMarker*> m_origMarkerList;

    /// linear curve
    QwtPlotCurve* m_linCurve;

    /// original curve
    QwtPlotCurve* m_origCurve;

    /// plot picker
    QwtPlotPicker* m_picker;

    /// edit mode state
    bool m_editModeActive;

    /// custom plot canvas
    GtControlScheduleCanvas* m_canvas;

    /// schedule
    GtControlSchedule* m_schedule;

    /// x axis description
    QString m_xDescr;

    /// y axis description
    QString m_yDescr;

    /// x axis unit
    QString m_xUnit;

    /// y axis unit
    QString m_yUnit;

    /// x Title
    QString m_xTitle;

    /// y Title
    QString m_yTitle;

signals:
    void pointAddedOrRemoved();

    void dataChanged();

    void editModeChanged(bool activate);

    void markerSelected(GtControlScheduleMarker* selectedMarker);

    void markersDeselected();
};

#endif // GTCONTROLSCHEDULEPLOT_H
