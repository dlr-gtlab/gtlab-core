/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_existingdirectoryproperty.cpp
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
