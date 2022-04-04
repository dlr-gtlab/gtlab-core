/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_processui.h"
#include "gt_icons.h"
#include "gt_regexp.h"

GtProcessUI::GtProcessUI()
{

}

QIcon
GtProcessUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::Icon::process16();
}

QStringList
GtProcessUI::openWith(GtObject* /*obj*/)
{
    return QStringList() << "GtProcessEditor";
}


bool
GtProcessUI::hasValidationRegExp()
{
    return true;
}

QRegExp
GtProcessUI::validatorRegExp()
{
    return GtRegExp::onlyLettersAndNumbersAndSpace();
}
