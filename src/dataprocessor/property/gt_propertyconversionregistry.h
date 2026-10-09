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

    /**
     * @brief registerConnectionCompatibility
     * Registration of of canConnect functions for a pair of two
     * property types
     * @param from
     * @param to
     * @param f
     */
    void registerConnectionCompatibility(
        QMetaObject from, QMetaObject to,
        gt::conversion::convert convert,
        gt::conversion::canConnect canConnect = {});

    void registerConnectionCompatibility(
        QMetaObject from, QMetaObject to,
        gt::conversion::canConnect canConnect);

    /**
     * @brief canConnectFunctions
     * Return the canConnection functions registered for the given pair
     * of property datatypes
     * @param from
     * @param to
     * @return
     */
    gt::conversion::canConnect canConnectFunction(
        QMetaObject const& from, QMetaObject const& to) const;

    QStringList converterAvailable(const QString& from) const;

private:
    GtPropertyConversionRegistry();
    ~GtPropertyConversionRegistry() = default;

    QList<GtPropertyConverter> canConvertHash;

};

GT_DATAMODEL_EXPORT GtPropertyConversionRegistry& gtPropConversion();

#endif // GT_PROPERTYCONVERSIONREGISTRY_H
