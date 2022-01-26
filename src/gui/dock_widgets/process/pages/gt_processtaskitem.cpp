/* GTlab - Gas Turbine laboratory
 * Source File: gt_processtaskitem.cpp
 * copyright 2009-2017 by DLR
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
