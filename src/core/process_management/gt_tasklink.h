/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKLINK_H
#define GTTASKLINK_H

#include "gt_core_exports.h"

#include "gt_calculator.h"

class GtTask;

/**
 * @brief The GtTaskLink class
 */
class GT_CORE_EXPORT GtTaskLink : public GtCalculator
{
    Q_OBJECT

public:
    /**
     * @brief GtTaskLink
     */
    Q_INVOKABLE GtTaskLink();

    /**
     * @brief target
     * @return
     */
    GtTask* target();

    bool run() override;

};

Q_DECLARE_METATYPE(GtTaskLink*)

#endif // GTTASKLINK_H
