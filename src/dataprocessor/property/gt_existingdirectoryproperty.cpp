/* GTlab - Gas Turbine laboratory
 * Source File: gt_existingdirectoryproperty.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.05.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_existingdirectoryproperty.h"

GtExistingDirectoryProperty::GtExistingDirectoryProperty(const QString& id,
                                                         const QString& name,
                                                         const QString& brief) :
    GtFileChooserProperty(id, name, brief, QStringList())
{

}
