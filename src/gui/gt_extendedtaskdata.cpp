/* GTlab - Gas Turbine laboratory
 * Source File: gt_extendedtaskdata.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_customprocesswizard.h"

#include "gt_extendedtaskdata.h"

GtExtendedTaskDataImpl::GtExtendedTaskDataImpl(const QMetaObject& metaData) :
    GtTaskDataImpl(metaData),
    wizard(nullptr)
{

}

GtExtendedTaskDataImpl::~GtExtendedTaskDataImpl()
{
    if (wizard)
    {
        delete wizard;
    }
}

bool
GtExtendedTaskDataImpl::isValid() const
{
    return GtTaskDataImpl::isValid();
}

GtExtendedTaskData
GtExtendedTaskDataImpl::newExtendedTaskData(const QMetaObject& metaData)
{
    return GtExtendedTaskData(new GtExtendedTaskDataImpl(metaData));
}
