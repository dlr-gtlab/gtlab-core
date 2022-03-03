/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
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
