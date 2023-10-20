/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODETYPEPROPERTY_H
#define GTMODETYPEPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"

/**
 * @brief The GtModeTypeProperty class
 */
class GT_DATAMODEL_EXPORT GtModeTypeProperty : public GtStringProperty
{
    Q_OBJECT

public:
    using GtProperty<QString>::operator=;

    /**
     * @brief GtModeTypeProperty
     * @param name
     * @param brief
     */
    GtModeTypeProperty(const QString& name,
                       const QString& brief);

};

#endif // GTMODETYPEPROPERTY_H
