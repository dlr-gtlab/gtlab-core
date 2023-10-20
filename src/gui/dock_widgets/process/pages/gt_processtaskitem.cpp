/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processtaskitem.cpp
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processcategoryitem.h"

#include "gt_processtaskitem.h"

GtProcessTaskItem::GtProcessTaskItem(GtTaskData taskData,
                                     GtProcessCategoryItem* parent) :
    GtAbstractProcessItem(parent),
    m_taskData(taskData)
{

}

GtTaskData
GtProcessTaskItem::taskData()
{
    return m_taskData;
}
