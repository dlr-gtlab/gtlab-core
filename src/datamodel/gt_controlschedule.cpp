/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_controlschedule.h"

GtControlSchedule::GtControlSchedule() :
    m_schedule("ControlSchedule",
               "Control Schedule",
               QStringLiteral("Schedule"),
               this)
{
    setFlag(UserDeletable);
    setFlag(UserRenamable);
    setDefault(false);
    setUserHidden(false);

    registerProperty(m_schedule, "Schedule");
}

bool
GtControlSchedule::isEmpty()
{
    if (getAxesNames().size() == 0)
    {
        // no Axes so no data in this schedule
        return true;
    }

    if (tabValsKeys().size() == 0)
    {
        // no key values so no data in this schedule
        return true;
    }

    return false;
}

bool
GtControlSchedule::isTransient()
{
    if (getAxesNames().size() == 1)
    {
        QString axName = getAxesNames().first();

        if (axName == "TIME" || axName == "Time" || axName == "time")
        {
            return true;
        }
    }

    return false;
}
