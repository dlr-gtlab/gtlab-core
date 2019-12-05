/* GTlab - Gas Turbine laboratory
 * Source File: gt_loop.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLOOP_H
#define GTLOOP_H

#include "gt_calculators_exports.h"

#include "gt_task.h"

/**
 * @brief The GtLoop class
 */
class GT_CALCULATORS_EXPORT GtLoop : public GtTask
{
    Q_OBJECT

public:
    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtLoop();

};

#endif // GTLOOP_H
