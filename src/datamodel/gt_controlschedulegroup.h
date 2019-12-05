/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulegroup.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.05.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTROLSCHEDULEGROUP_H
#define GTCONTROLSCHEDULEGROUP_H

#include "gt_datamodel_exports.h"
#include "gt_objectgroup.h"

#include "gt_controlschedule.h"

class GT_DATAMODEL_EXPORT GtControlScheduleGroup : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtControlScheduleGroup
     */
    Q_INVOKABLE GtControlScheduleGroup(GtObject* parent = NULL);

    /**
     * @brief schedules
     * @return
     */
    QList<GtControlSchedule*> schedules();

    /**
     * @brief schedule
     * @param name
     * @return
     */
    GtControlSchedule* schedule(QString name);

    /**
     * @brief scheduleNames
     * @return
     */
    QStringList scheduleNames();

    /**
     * @brief scheduleNamesNotEmpty: Yields only names of schedules
     *                               which have data in the table
     * @return
     */
    QStringList scheduleNamesNotEmpty(bool transient);

    /**
     * @brief scheduleNamesNotEmpty
     * @return
     */
    QStringList scheduleNamesNotEmpty();

    /**
     * @brief existingSchedules
     * @return
     */
    QStringList existingSchedules();
};

#endif // GTCONTROLSCHEDULEGROUP_H
