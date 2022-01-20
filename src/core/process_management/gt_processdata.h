/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSDATA_H
#define GTPROCESSDATA_H

#include "gt_core_exports.h"
#include "gt_objectgroup.h"

class GtTask;

/**
 * @brief The GtProcessData class
 */
class GT_CORE_EXPORT GtProcessData : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessData
     */
    Q_INVOKABLE GtProcessData();

    /**
     * @brief processList
     * @return
     */
    QList<GtTask*> processList();

    /**
     * @brief findProcess
     * @param val
     * @return
     */
    GtTask* findProcess(const QString& val);

};

#endif // GTPROCESSDATA_H
