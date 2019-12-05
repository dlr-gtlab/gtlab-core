/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTGROUP_H
#define GTOBJECTGROUP_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

/**
 * @brief The GtObjectGroup class
 */
class GT_DATAMODEL_EXPORT GtObjectGroup : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief GtObjectGroup
     * @param parent
     */
    Q_INVOKABLE GtObjectGroup(GtObject* parent = NULL);

};

#endif // GTOBJECTGROUP_H
