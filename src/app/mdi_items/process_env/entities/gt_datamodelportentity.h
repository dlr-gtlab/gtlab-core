/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDATAMODELPORTENTITY_H
#define GTDATAMODELPORTENTITY_H

#include "gt_propertyportentity.h"

/**
 * @brief The GtDataModelPortEntity class
 */
class GtDataModelPortEntity : public GtPropertyPortEntity
{
    Q_OBJECT

public:
    /**
     * @brief GtDataModelPortEntity
     */
    explicit GtDataModelPortEntity(QGraphicsItem* parent = Q_NULLPTR);

};

#endif // GTDATAMODELPORTENTITY_H
