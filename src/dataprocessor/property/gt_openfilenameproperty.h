/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_openfilenameproperty.h
 *
 *  Created on: 12.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOPENFILENAMEPROPERTY_H
#define GTOPENFILENAMEPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_filechooserproperty.h"

/**
 * @brief The GtOpenFileNameProperty class
 */
class GT_DATAMODEL_EXPORT GtOpenFileNameProperty : public GtFileChooserProperty
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     *
     * Note: the file filters except two different styles:
     *
     *  1. Old-style filters (e.g. ".txt")
     *  2. Qt-style filters (e.g. "Textfile (*.txt)");
     *
     * The filter styles can be combined arbitrarily
     *
     * @param Property identification string.
     * @param Property name.
     * @param Property description.
     * @param File extension filter list (eg. ".exe", or "Executable (*.exe)")
     */
    GtOpenFileNameProperty(const QString& id,
                           const QString& name,
                           const QString& brief,
                           const QStringList& fileFilter);

};

#endif // GTOPENFILENAMEPROPERTY_H
