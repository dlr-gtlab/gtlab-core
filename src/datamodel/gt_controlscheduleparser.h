/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTCONTROLSCHEDULEPARSER_H
#define GTCONTROLSCHEDULEPARSER_H

#include "gt_controlschedule.h"

class QDomElement;

class GT_DATAMODEL_EXPORT GtControlScheduleParser : public QObject
{

public:
    /**
     * @brief GtControlScheduleParser
     */
    GtControlScheduleParser();

    /**
     * @brief fromDomElement
     * @param root
     * @param success
     * @return
     */
    GtControlSchedule* fromDomElement(QDomElement& root,
                                      bool& success);

    bool parseTable(QDomElement& root, GtControlSchedule* map);

private:

    bool parseSchedule(QDomElement& root, GtControlSchedule* cs);
};

#endif // GTCONTROLSCHEDULEPARSER_H
