/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef TESTCALCULATOR_H
#define TESTCALCULATOR_H

#include "gt_calculator.h"
#include "gt_doubleproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"


class TestCalculator : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE TestCalculator();

    bool run() override;

    GtDoubleProperty m_value;

    GtDoubleProperty m_result;

    GtObjectLinkProperty m_objectLink;

    GtObjectPathProperty m_objectPath;

};

#endif // TESTCALCULATOR_H
