/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    return GtGUI::icon("calculatorIcon_16.png");
}

bool
GtCalculatorUI::hasValidationRegExp()
{
    return true;
}

QRegExp
GtCalculatorUI::validatorRegExp()
{
    return GtRegExp::onlyLettersAndNumbersAndSpace();
}
