/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_label.h
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
     * @param id - Label identification string initialization
     */
    Q_INVOKABLE explicit GtLabel(const QString& id = QString());

};

#endif // GTLABEL_H
