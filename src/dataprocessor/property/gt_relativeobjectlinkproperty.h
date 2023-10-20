/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_relativeobjectlinkproperty.h
 *
 *  Created on: 19.12.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTRELATIVEOBJECTLINKPROPERTY_H
#define GTRELATIVEOBJECTLINKPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"

/**
 * @brief The GtRelativeObjectLinkProperty class
 */
class GT_DATAMODEL_EXPORT GtRelativeObjectLinkProperty :
        public GtStringProperty
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Property identification string.
     * @param Property name.
     * @param Property description.
     * @param uuid)
     */
    Q_INVOKABLE GtRelativeObjectLinkProperty(const QString& id,
                                             const QString& name,
                                             const QString& brief,
                                             const QString& uuid);

};

#endif // GTRELATIVEOBJECTLINKPROPERTY_H
