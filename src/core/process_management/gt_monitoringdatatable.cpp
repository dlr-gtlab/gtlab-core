/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_monitoringdatatable.cpp
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include "gt_monitoringdatatable.h"

GtMonitoringDataTable::GtMonitoringDataTable()
{

}

void
GtMonitoringDataTable::clear()
{
    m_data.clear();
}

int
GtMonitoringDataTable::size() const
{
    return m_data.size();
}

bool
GtMonitoringDataTable::append(int iteration,
                              const GtMonitoringDataSet& dataSet)
{
    // validate iteration number
    if (iteration <= 0)
    {
        return false;
    }

    // get current data size
    const int dataSize = m_data.size();

    // validate iteration number with current data size
    if (iteration <= dataSize)
    {
        return false;
    }

    // fill missing iterations using empty data sets
    for (int i = dataSize; i < (iteration - 1); i++)
    {
        m_data << GtMonitoringDataSet();
    }

    // append new data set
    m_data << dataSet;

    return true;
}

QVector<QVariant>
GtMonitoringDataTable::getData(const QString& compUuid, const QString& propId)
{
    // check strings
    if (propId.isEmpty() || compUuid.isEmpty())
    {
        gtError() << QObject::tr("Monitoring item invalid!");
        return QVector<QVariant>();
    }

    // create and initialize return vector
    QVector<QVariant> retval(size(), QVariant());

    for (int i = 0; i < m_data.size(); i++)
    {
        if (m_data[i].contains(compUuid))
        {
            GtMonitoringData mdata = m_data[i].getData(compUuid);

            if (mdata.contains(propId))
            {
                QVariant mval = mdata.getData(propId);

                retval[i] = mval;
            }
        }
    }

    return retval;
}
