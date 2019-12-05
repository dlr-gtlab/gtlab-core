/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulegroup.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.05.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_controlschedulegroup.h"

GtControlScheduleGroup::GtControlScheduleGroup(GtObject* parent) :
    GtObjectGroup(parent)
{
    setObjectName("Schedules");
}

QList<GtControlSchedule*>
GtControlScheduleGroup::schedules()
{
    return findDirectChildren<GtControlSchedule*>();
}

GtControlSchedule*
GtControlScheduleGroup::schedule(QString name)
{
    if (scheduleNames().contains(name))
    {
        return findDirectChild<GtControlSchedule*>(name);
    }

    return Q_NULLPTR;
}

QStringList
GtControlScheduleGroup::scheduleNames()
{
    QStringList retval;

    foreach (GtControlSchedule* s, schedules())
    {
        if (s == Q_NULLPTR)
        {
            continue;
        }

        retval.append(s->objectName());
    }

    return retval;
}

QStringList
GtControlScheduleGroup::scheduleNamesNotEmpty(bool transient)
{
    QStringList retval;

    foreach (GtControlSchedule* s, schedules())
    {
        if (s == Q_NULLPTR)
        {
            continue;
        }

        if (s->isEmpty())
        {
            continue;
        }

        if (s->isTransient() == transient)
        {
            retval.append(s->objectName());
        }
    }

    return retval;
}

QStringList
GtControlScheduleGroup::scheduleNamesNotEmpty()
{
    QStringList retval;

    foreach (GtControlSchedule* s, schedules())
    {
        if (s == Q_NULLPTR)
        {
            continue;
        }

        if (s->isEmpty())
        {
            continue;
        }

        retval.append(s->objectName());
    }

    return retval;
}

QStringList
GtControlScheduleGroup::existingSchedules()
{
    QStringList retval;

    foreach (GtControlSchedule* cs,
             findDirectChildren<GtControlSchedule*>())
    {
        if (cs != Q_NULLPTR)
        {
            retval.append(cs->objectName());
        }
    }

    return retval;
}
