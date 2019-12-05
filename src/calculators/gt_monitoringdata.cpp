/* GTlab - Gas Turbine laboratory
 * Source File: gt_monitoringdata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_monitoringdata.h"

GtMonitoringData::GtMonitoringData()
{

}

bool
GtMonitoringData::addData(const QString& pId, const QVariant& pVal)
{
    // check identification string
    if (pId.isEmpty())
    {
        qWarning() << QStringLiteral("Could not add data! Id is empty!");
        return false;
    }

    // check whether same id already exists
    if (m_data.contains(pId))
    {
        qWarning() << QStringLiteral("Could not add data! Id already exists!");
        return false;
    }

    // insert data
    m_data.insert(pId, pVal);

    // return success
    return true;
}

const QMap<QString, QVariant>&
GtMonitoringData::data()
{
    return m_data;
}

bool
GtMonitoringData::contains(const QString& pId)
{
    return m_data.contains(pId);
}

QVariant
GtMonitoringData::getData(const QString& pId)
{
    return m_data.value(pId);
}
