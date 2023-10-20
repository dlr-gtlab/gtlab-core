/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_openfilenameproperty.cpp
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
