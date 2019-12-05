/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_objectgroup.h"

GtObjectGroup::GtObjectGroup(GtObject* parent) : GtObject(parent)
{
    setObjectName("Group");
}

