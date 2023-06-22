/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    Q_INVOKABLE explicit GtObjectGroup(GtObject* parent = nullptr);

};

#endif // GTOBJECTGROUP_H
