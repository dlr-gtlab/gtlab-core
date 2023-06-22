/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_calculatorui.h"
#include "gt_icons.h"
#include "gt_regexp.h"

GtCalculatorUI::GtCalculatorUI()
{

}

QIcon
GtCalculatorUI::icon(GtObject* /*obj*/) const
{
    return gt::gui::icon::calculator();
}

bool
GtCalculatorUI::hasValidationRegExp()
{
    return true;
}

QRegExp
GtCalculatorUI::validatorRegExp()
{
    return gt::re::onlyLettersAndNumbersAndSpace();
}
