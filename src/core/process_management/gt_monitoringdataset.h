/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_monitoringdataset.h
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
#include "gt_version.h"

/**
 * @brief The GtMonitoringDataSet class
 */
class GT_CORE_EXPORT GtMonitoringDataSet
{
public:
    /**
     * @brief Constructor.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    GtMonitoringDataSet();

    /**
     * @brief Inserts data to data set.
     * @param Process component UUID.
     * @param Monitoring data.
     * @return Returns true if data was added, otherwise returns false.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    bool insert(const QString& uuid, const GtMonitoringData& monData);

    /**
     * @brief Returns size of data.
     * @return Size of data.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    int size();

    /**
     * @brief Returns true if data is empty, otherwise returns false.
     * @return Whether data is empty or not.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    bool isEmpty();

    /**
     * @brief Returns true if process component uuid exists, otherwise false
     * is returned.
     * @param Process component UUID.
     * @return  Whether process component uuid exists or not.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    bool contains(const QString& uuid);

    /**
     * @brief Returns monitoring data for given process component UUID.
     * @param Process component UUID.
     * @return Process monitoring data.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    GtMonitoringData getData(const QString& uuid);

private:
    /// Monitoring data.
    QMap<QString, GtMonitoringData> m_data;

};

#endif // GTMONITORINGDATASET_H
