/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processtaskitem.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSTASKITEM_H
#define GT_PROCESSTASKITEM_H

#include "gt_abstractprocessitem.h"

#include "gt_taskdata.h"

class GtProcessCategoryItem;

/**
 * @brief The GtProcessTaskItem class
 */
class GtProcessTaskItem : public GtAbstractProcessItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Task data.
     * @param Parent.
     */
    GtProcessTaskItem(GtTaskData taskData,
                      GtProcessCategoryItem* parent);

    /**
     * @brief Returns task data.
     * @return Task data.
     */
    GtTaskData taskData();

private:
    /// Task data.
    GtTaskData m_taskData;

};

#endif // GT_PROCESSTASKITEM_H
