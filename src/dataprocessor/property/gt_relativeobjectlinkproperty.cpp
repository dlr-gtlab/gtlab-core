/* GTlab - Gas Turbine laboratory
 * Source File: gt_relativeobjectlinkproperty.cpp
 * copyright 2009-2017 by DLR
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
