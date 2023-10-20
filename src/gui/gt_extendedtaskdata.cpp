/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_extendedtaskdata.cpp
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
    delete wizard;
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
