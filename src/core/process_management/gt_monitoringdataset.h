/* GTlab - Gas Turbine laboratory
 * Source File: gt_monitoringdataset.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMONITORINGDATASET_H
#define GTMONITORINGDATASET_H

#include "gt_core_exports.h"

#include <QMap>
#include <QString>

#include "gt_monitoringdata.h"

/**
 * @brief The GtMonitoringDataSet class
 */
class GT_CORE_EXPORT GtMonitoringDataSet
{
public:
    /**
     * @brief Constructor.
     */
    GtMonitoringDataSet();

    /**
     * @brief Inserts data to data set.
     * @param Process component UUID.
     * @param Monitoring data.
     * @return Returns true if data was added, otherwise returns false.
     */
    bool insert(const QString& uuid, const GtMonitoringData& monData);

    /**
     * @brief Returns size of data.
     * @return Size of data.
     */
    int size();

    /**
     * @brief Returns true if data is empty, otherwise returns false.
     * @return Whether data is empty or not.
     */
    bool isEmpty();

    /**
     * @brief Returns true if process component uuid exists, otherwise false
     * is returned.
     * @param Process component UUID.
     * @return  Whether process component uuid exists or not.
     */
    bool contains(const QString& uuid);

    /**
     * @brief Returns monitoring data for given process component UUID.
     * @param Process component UUID.
     * @return Process monitoring data.
     */
    GtMonitoringData getData(const QString& uuid);

private:
    /// Monitoring data.
    QMap<QString, GtMonitoringData> m_data;

};

#endif // GTMONITORINGDATASET_H
