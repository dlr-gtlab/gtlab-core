/* GTlab - Gas Turbine laboratory
 * Source File: gt_label.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_label.h"

GtLabel::GtLabel(const QString& id)
{
    setObjectName(id);

    setFlag(GtObject::UserRenamable, true);
}
