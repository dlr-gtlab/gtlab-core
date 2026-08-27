/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_MEMENTO_PROJECT_CALCULATOR_H
#define TEST_MEMENTO_PROJECT_CALCULATOR_H

#include "gt_calculator.h"
#include "gt_intproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_stringproperty.h"

class TestMementoProjectCalculator final : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE TestMementoProjectCalculator();

    bool run() override;

private:
    GtObjectLinkProperty m_target;
    GtIntProperty m_inputValue;
    GtStringProperty m_sideEffectFile;
};

#endif // TEST_MEMENTO_PROJECT_CALCULATOR_H
