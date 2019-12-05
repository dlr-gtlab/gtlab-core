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
#include "gt_application.h"
#include "gt_processeditor.h"
#include "gt_regexp.h"

GtProcessUI::GtProcessUI()
{

}

QIcon
GtProcessUI::icon(GtObject* /*obj*/) const
{
    return gtApp->icon(QStringLiteral("processIcon_16.png"));
}

QStringList
GtProcessUI::openWith(GtObject* /*obj*/)
{
    return QStringList() << GtProcessEditor::staticMetaObject.className();
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
