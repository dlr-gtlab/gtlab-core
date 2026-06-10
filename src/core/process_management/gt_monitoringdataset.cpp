/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_monitoringdataset.cpp
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_monitoringdataset.h"

GtMonitoringDataSet::GtMonitoringDataSet()
{

}

bool GtMonitoringDataSet::insert(const QString& uuid,
                            const GtMonitoringData& monData)
{
    // check whether data already exists
    if (m_data.contains(uuid))
    {
        qWarning() << QStringLiteral(
                          "Could not add data! UUID already exists!");
        return false;
    }

    // insert data
    m_data.insert(uuid, monData);

    // return success
    return true;
}

int
GtMonitoringDataSet::size()
{
    return m_data.size();
}

bool
GtMonitoringDataSet::isEmpty()
{
    return m_data.isEmpty();
}

bool
GtMonitoringDataSet::contains(const QString& uuid)
{
    return m_data.contains(uuid);
}

GtMonitoringData
GtMonitoringDataSet::getData(const QString& uuid)
{
    return m_data.value(uuid);
}
