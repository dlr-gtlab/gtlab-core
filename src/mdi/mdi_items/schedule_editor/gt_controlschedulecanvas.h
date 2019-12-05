/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulecanvas.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 26.02.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTROLSCHEDULECANVAS_H
#define GTCONTROLSCHEDULECANVAS_H

#include "qwt_plot_canvas.h"

class GtControlSchedulePlot;
class GtControlScheduleMarker;
class QwtPlotMarker;

/**
 * @brief The GtControlScheduleCanvas class
 */
class GtControlScheduleCanvas : public QwtPlotCanvas
{
    Q_OBJECT
public:
    /**
     * @brief GtControlScheduleCanvas
     */
    GtControlScheduleCanvas(GtControlSchedulePlot* plot);

    /**
     * @brief mouse move event implementation
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event);

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
     * @brief activates the add point mode including all necessary settings
     */
    void activateAddPointState(bool activate = true);

    /**
     * @brief activates the data probe mode including all necessary settings
     * @param activate
     */
    void activateDataProbeState(bool activate = true);

    /**
     * @brief retuns true in case that add point mode is active
     * @return
     */
    bool addPointActive();

    /**
     * @brief return true in case that data probe mode is active
     * @return
     */
    bool dataProbeActive();

private:
    /// qwt plot
    GtControlSchedulePlot* m_plot;

    /// selected marker
    GtControlScheduleMarker* m_selectedMarker;

    /// ready to move flag
    bool m_readyToMove;

    /// add point flag
    bool m_addPointActive;

    /// data probe flag
    bool m_dataProbeActive;

    /// data probe marker
    QwtPlotMarker* m_tmpMarker;

    /**
     * @brief returns the closest plot marker to a certain position and the
     * related distance
     * @param pos position
     * @param dist distance
     * @return
     */
    GtControlScheduleMarker* closestMarker(const QPoint& pos, double& dist);

    /**
     * @brief context menu definition
     */
    void rightClickContextMenu();

    /**
     * @brief onModifyRequest
     */
    void onModifyRequest();

    /**
     * @brief point deletion when requested by user
     */
    void onDeletePointRequest();

    /**
     * @brief signal emitation to change edit mode
     */
    void onChangeEditMode();

    /**
     * @brief signal emitation to change add point mode
     */
    void onAddPointRequest();

    /**
     * @brief onModAxisRequest
     */
    void onModAxisRequest();

    /**
     * @brief signal emitation to change data probe mode
     */
    void onDataProbeRequest();

    /**
     * @brief signal emitation to revert schedule data
     */
    void onRevertRequest();

    /**
     * @brief signal emitation to save schedule data
     */
    void onSaveRequest();

signals:
    /**
     * @brief signal to change edit mode
     */
    void changeEditMode();

    /**
     * @brief signal to change add point mode
     */
    void changeAddPointMode();

    /**
     * @brief changeAxisData
     */
    void changeAxisData();

    /**
     * @brief signal to change data probe mode
     */
    void changeDataProbeMode();

    /**
     * @brief signal to revert schedule data
     */
    void revertAndDiscard();

    /**
     * @brief signal to save schedule data
     */
    void save();

    /**
     * @brief signal emitted when a point has been moved
     */
    void pointMoved();

    /**
     * @brief signal emitted when a data probe is taken
     */
    void dataProbeTaken(const QPointF point);
};

#endif // GTCONTROLSCHEDULECANVAS_H
