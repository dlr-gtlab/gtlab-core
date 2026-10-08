/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GT_PROPERTYCONVERSIONREGISTRY_H
#define GT_PROPERTYCONVERSIONREGISTRY_H

#include "gt_datamodel_exports.h"
#include "gt_propertyconverter.h"
#include <QString>
#include <QMultiHash>

class GT_DATAMODEL_EXPORT GtPropertyConversionRegistry
{

public:
    // Copy and assignment und Zuweisen löschen
    GtPropertyConversionRegistry(
        const GtPropertyConversionRegistry&) = delete;

    GtPropertyConversionRegistry& operator=(
        const GtPropertyConversionRegistry&) = delete;

    static GtPropertyConversionRegistry& getInstance();

    using ClassName = QString;
    QMultiHash<ClassName, GtPropertyConverter> canConvertHash;

private:
    GtPropertyConversionRegistry() = default;
    ~GtPropertyConversionRegistry() = default;


};

GtPropertyConversionRegistry& gtPropConversion();

#endif // GT_PROPERTYCONVERSIONREGISTRY_H
