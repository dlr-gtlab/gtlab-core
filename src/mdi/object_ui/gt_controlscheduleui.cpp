/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include <QFont>
#include <QIcon>

#include "gt_controlscheduleui.h"
#include "gt_controlscheduleeditor.h"
#include "gt_application.h"
#include "gt_regexp.h"

GtControlScheduleUI::GtControlScheduleUI()
{
    addSingleAction(tr("Activate"),
                    QStringLiteral("checkSmallIcon_16.png"),
                    QStringLiteral("activate"),
                    QStringLiteral("isDeactivated"),
                    QStringLiteral("isDeactivated"));

    addSingleAction(tr("Deactivate"),
                    QStringLiteral("crossIcon_16.png"),
                    QStringLiteral("deactivate"),
                    QStringLiteral("isActivated"),
                    QStringLiteral("isActivated"));
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

bool
GtControlScheduleUI::isActivated(GtObject* obj)
{
    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(obj);

    if (!schedule)
    {
        gtError() << "GtControlScheduleUI::isActivate():: "
                     "Could not cast schedule " << obj->objectName();

        return false;
    }

    return schedule->isActive();
}

bool
GtControlScheduleUI::isDeactivated(GtObject* obj)
{
    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(obj);

    if (!schedule)
    {
        gtError() << "GtControlScheduleUI::isDeactivated():: "
                     "Could not cast schedule " << obj->objectName();

        return false;
    }

    return !schedule->isActive();
}

void
GtControlScheduleUI::activate(GtObject* obj)
{
    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(obj);

    if (!schedule)
    {
        gtError() << "GtControlScheduleUI::activate():: "
                  << "Could not cast schedule " << obj->objectName();
        return;
    }

    schedule->setActive(true);
}

void
GtControlScheduleUI::deactivate(GtObject* obj)
{
    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(obj);

    if (!schedule)
    {
        gtError() << "GtControlScheduleUI::deactivate():: "
                     "Could not cast schedule " << obj->objectName();
        return;
    }

    schedule->setActive(false);
}


QVariant
GtControlScheduleUI::specificData(GtObject* obj, int role, int column) const
{
    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(obj);

    if (schedule == nullptr)
    {
        return QVariant();
    }

    if (column == 0)
    {
        switch (role)
        {
            case Qt::FontRole:
            {
                if (!schedule->isActive())
                {
                    QFont font;
                    font.setItalic(true);
                    font.setStrikeOut(true);

                    return font;
                }

                break;
            }
        }
    }

    return QVariant();
}
