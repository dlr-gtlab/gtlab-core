/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTCONTROLSCHEDULE_H
#define GTCONTROLSCHEDULE_H

#include "gt_datamodel_exports.h"
#include "gt_table.h"
#include "gt_controlscheduleproperty.h"


/**
 * @brief The GtControlSchedule class
 */
class GT_DATAMODEL_EXPORT GtControlSchedule : public GtTable
{
    Q_OBJECT

public:
    /**
     * @brief GtControlSchedule
     */
    Q_INVOKABLE GtControlSchedule();

    /**
     * @brief isEmpty
     * @return
     */
    bool isEmpty();

    /**
     * @brief isTransient
     * @return
     */
    bool isTransient();

private:

    /// Schedule property
    GtControlScheduleProperty m_schedule;
};

#endif // GTCONTROLSCHEDULE_H
