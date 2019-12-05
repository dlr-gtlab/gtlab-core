/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_controlscheduleui.h"
#include "gt_controlscheduleeditor.h"
#include "gt_application.h"
#include "gt_regexp.h"
#include <QIcon>

GtControlScheduleUI::GtControlScheduleUI()
{

}

QIcon
GtControlScheduleUI::icon(GtObject* /*obj*/) const
{
    return gtApp->icon("schedules3Icon_16.png");
}

QStringList
GtControlScheduleUI::openWith(GtObject* /*obj*/)
{
    return QStringList()
            << GtControlScheduleEditor::staticMetaObject.className();
}

bool
GtControlScheduleUI::hasValidationRegExp()
{
    return true;
}


QRegExp
GtControlScheduleUI::validatorRegExp()
{
    return GtRegExp::onlyLettersAndNumbersAndDot();
}


