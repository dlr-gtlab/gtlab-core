/* GTlab - Gas Turbine laboratory
 * Source File: gt_monitoringdatatable.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMONITORINGDATATABLE_H
#define GTMONITORINGDATATABLE_H

#include "gt_core_exports.h"

#include <QVector>

#include "gt_monitoringdataset.h"

/**
 * @brief The GtMonitoringDataTable class
 */
class GT_CORE_EXPORT GtMonitoringDataTable
{
public:
    /**
     * @brief Constructor.
     */
    GtMonitoringDataTable();

    /**
     * @brief Clears entire data set list.
     */
    void clear();

    /**
     * @brief Returns size of monitoring data table.
     * @return Size of monitoring data table.
     */
    int size();

    /**
     * @brief Appends data set to data table. Returns false if iteration number
     * is invalid or smaller as data table size.
     * Fills up missing iteration numbers with empty data sets.
     * @param Iteration number corresponding to given data set.
     * @param Monitoring data set.
     */
    bool append(int iteration, const GtMonitoringDataSet& dataSet);

    /**
     * @brief Returns data based on given process component uuid and property
     * identification string.
     * @param Process component UUID
     * @param Property identifications tring
     * @return Monitoring data.
     */
    QVector<QVariant> getData(const QString& compUuid, const QString& propId);

private:
    /// List of monitoring data sets.
    QVector<GtMonitoringDataSet> m_data;

};

#endif // GTMONITORINGDATATABLE_H
