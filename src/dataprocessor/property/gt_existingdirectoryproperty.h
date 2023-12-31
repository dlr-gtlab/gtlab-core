/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_existingdirectoryproperty.h
 *
 *  Created on: 30.05.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_EXISTINGDIRECTORYPROPERTY_H
#define GT_EXISTINGDIRECTORYPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_filechooserproperty.h"

/**
 * @brief The GtExistingDirectoryProperty class
 */
class GT_DATAMODEL_EXPORT GtExistingDirectoryProperty : public GtFileChooserProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtExistingDirectoryProperty
     * @param id
     * @param name
     * @param brief
     */
    GtExistingDirectoryProperty(const QString& id,
                                const QString& name,
                                const QString& brief);

};

#endif // GT_EXISTINGDIRECTORYPROPERTY_H
