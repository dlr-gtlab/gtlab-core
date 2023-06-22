/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_relativeobjectlinkproperty.cpp
 *
 *  Created on: 19.12.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_relativeobjectlinkproperty.h"

GtRelativeObjectLinkProperty::GtRelativeObjectLinkProperty(
        const QString& id, const QString& name,
        const QString& brief, const QString& uuid) :
    GtStringProperty(id, name, brief, uuid)
{
    hide();
}
