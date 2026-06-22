/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_monitoringdata.h
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMONITORINGDATA_H
#define GTMONITORINGDATA_H

#include "gt_core_exports.h"

#include <QString>
#include <QVariant>
#include <QMap>
#include "gt_version.h"

/**
 * @brief The GtMonitoringData class
 */
class GT_CORE_EXPORT GtMonitoringData
{
public:
    /**
     * @brief Constructor.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    GtMonitoringData();

    /**
     * @brief Adds a data set to monitoring data map.
     * @param Property identification string.
     * @param Property value as variant.
     * @return Returns true if data was successfully added, otherwise returns
     * false.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    bool addData(const QString& pId, const QVariant& pVal);

    /**
     * @brief Returns monitoring data map.
     * @return Monitoring data map.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    const QMap<QString, QVariant>& data();

    /**
     * @brief Returns true if property identification string exists,
     * otherwise false is returned.
     * @param Property identification string.
     * @return  Whether property identification string exists or not.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    bool contains(const QString& pId);

    /**
     * @brief Returns monitoring data for given property identification string.
     * @param Property identification string.
     * @return Process monitoring data.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Not supported anymore")
    QVariant getData(const QString& pId);

private:
    /// Monitoring data
    QMap<QString, QVariant> m_data;

};

#endif // GTMONITORINGDATA_H
