/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTRESULT_H
#define GTRESULT_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

class GtLabel;

/**
 * @brief The GtResult class
 */
class GT_DATAMODEL_EXPORT GtResult : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief GtResult
     */
    Q_INVOKABLE GtResult();

    /**
     * @brief labels
     * @return
     */
    QList<GtLabel*> labels();

    /**
     * @brief labelNames
     * @return
     */
    QStringList labelNames();

};

#endif // GTRESULT_H
