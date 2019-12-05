/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoring.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMONITORING_H
#define GTPROCESSMONITORING_H

#include "gt_mdiitem.h"

#include <QPointer>

class GtProcessMonitoringView;
class GtProcessMonitoringModel;
class GtProcessMonitoringItem;
class GtTask;
class QwtPlot;
class QwtPlotItem;
class QwtPlotLegendItem;

/**
 * @brief The GtProcessMonitoring class
 */
class GtProcessMonitoring : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE GtProcessMonitoring();

    /**
     *
     */
    ~GtProcessMonitoring();

    /**
     * @brief setData
     * @param obj
     */
    virtual void setData(GtObject* obj) Q_DECL_OVERRIDE;

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

private:
    /// Process monitoring model
    GtProcessMonitoringModel* m_model;

    /// Tree view widget
    GtProcessMonitoringView* m_view;

    /// Analyser widget
    QwtPlot* m_analyzer;

    /// Root task
    QPointer<GtTask> m_task;

    QList<QwtPlotItem*> m_plotItems;

    QwtPlotLegendItem* m_legend;

    /**
     * @brief createPlotCurve
     * @param compUuid
     * @param propId
     */
    void createPlotCurve(const QString& compUuid, const QString& propId,
                         GtProcessMonitoringItem* item,
                         GtProcessMonitoringItem* parentItem);

    /**
     * @brief clearPlotItems
     *  detaches and deletes all elements of the m_plotItems member list
     *  clears list afterwards
     */
    void clearPlotItems();

private slots:
    /**
     * @brief Called after selection has changed to show selected monitoring
     * data in analyser widget.
     */
    void onSelectionChanged();

    /**
     * @brief Called when monitoring data is available.
     */
    void onMonitoringDataAvailable();

};

#endif // GTPROCESSMONITORING_H
