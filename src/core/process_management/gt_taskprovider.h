/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskprovider.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKPROVIDER_H
#define GTTASKPROVIDER_H

#include "gt_core_exports.h"

#include "gt_abstractprocessprovider.h"

class GtTask;

/**
 * @brief The GtTaskProvider class
 */
class GT_CORE_EXPORT GtTaskProvider : public GtAbstractProcessProvider
{
public:
    /**
     * @brief Constructor.
     * @param initTask Task for provider initialization
     */
    explicit GtTaskProvider(GtTask* initTask = Q_NULLPTR);

};

#endif // GTTASKPROVIDER_H
