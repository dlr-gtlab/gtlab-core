/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_loop.h
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLOOP_H
#define GTLOOP_H

#include "gt_core_exports.h"

#include "gt_task.h"

/**
 * @brief The GtLoop class
 */
class GT_CORE_EXPORT GtLoop : public GtTask
{
    Q_OBJECT

public:
    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtLoop();

};

#endif // GTLOOP_H
