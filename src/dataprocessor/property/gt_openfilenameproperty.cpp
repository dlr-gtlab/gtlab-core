/* GTlab - Gas Turbine laboratory
 * Source File: gt_openfilenameproperty.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 12.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_openfilenameproperty.h"

GtOpenFileNameProperty::GtOpenFileNameProperty(const QString& id,
                                               const QString& name,
                                               const QString& brief,
                                               const QStringList& fileFilter) :
    GtFileChooserProperty(id, name, brief, fileFilter)
{

}
