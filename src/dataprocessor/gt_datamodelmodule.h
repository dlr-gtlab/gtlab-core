/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDATAMODELMODULE_H
#define GTDATAMODELMODULE_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

/**
 * @brief The GtDatamodelModule class
 */
class GT_DATAMODEL_EXPORT GtDatamodelModule : public GtObject
{
    Q_OBJECT

protected:
    /**
     * @brief GtDatamodelModule
     */
    GtDatamodelModule();

};

#endif // GTDATAMODELMODULE_H
