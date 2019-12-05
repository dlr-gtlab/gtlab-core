/* GTlab - Gas Turbine laboratory
 * Source File: gt_label.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABEL_H
#define GTLABEL_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

/**
 * @brief The GtLabel class
 */
class GT_DATAMODEL_EXPORT GtLabel : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param Label identification string initialization
     */
    Q_INVOKABLE GtLabel(const QString& id = QString());

};

#endif // GTLABEL_H
