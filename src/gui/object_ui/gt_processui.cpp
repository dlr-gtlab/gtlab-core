/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    return gt::gui::icon::process();
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
    return gt::re::onlyLettersAndNumbersAndSpace();
}
