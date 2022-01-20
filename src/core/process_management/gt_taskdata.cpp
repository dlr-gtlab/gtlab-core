/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskdataimpl.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QObject>

#include "gt_taskdata.h"

GtTaskDataImpl::GtTaskDataImpl(const QMetaObject& metaData) :
    m_metaData(metaData)
{
    description.clear();
    status = PROTOTYPE;
}

const QMetaObject&
GtTaskDataImpl::metaData() const
{
    return m_metaData;
}

bool
GtTaskDataImpl::isValid() const
{
    if (version.isNull())
    {
        return false;
    }

    if (id.isEmpty())
    {
        return false;
    }

    QObject* obj = m_metaData.newInstance();

    if (obj == Q_NULLPTR)
    {
        return false;
    }

    delete obj;

    return true;
}

GtTaskData
GtTaskDataImpl::newTaskData(const QMetaObject& metaData)
{
    return GtTaskData(new GtTaskDataImpl(metaData));
}
