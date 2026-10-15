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
    // Delete copy and assignment functions
    GtPropertyConversionRegistry(const GtPropertyConversionRegistry&) = delete;

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
        QMetaObject from, QMetaObject to, gt::conversion::convert convert,
        gt::conversion::isCompatible canConnect = {});

    template <typename Converter>
    void registerConnectionCompatibility(
        const QMetaObject& from, const QMetaObject& to, Converter&& conversion,
        gt::conversion::isCompatible canConnect)
    {
        auto erasedConversion =
            gt::conversion::eraseConverter(std::forward<Converter>(conversion));

        registerConnectionCompatibility(from, to, std::move(erasedConversion),
                                        std::move(canConnect));
    }

    /**
     * @brief canConnectFunctions
     * Return the canConnection functions registered for the given pair
     * of property datatypes
     * @param from
     * @param to
     * @return
     */
    gt::conversion::isCompatible isCompatibleFunction(
        QMetaObject const& from, QMetaObject const& to) const;

    /**
     * @brief convertFunction
     * Return the conversion functions registered for the given pair
     * of property datatypes
     * @param from
     * @param to
     * @return
     */
    gt::conversion::convert convertFunction(QMetaObject const& from,
                                            QMetaObject const& to) const;

    const GtPropertyConverter* findConverterWithInheritance(
        const QMetaObject& from, const QMetaObject& to) const;


    QStringList converterAvailable(const QString& from) const;

    bool connectionCheckAvailable(GtAbstractProperty const& a,
                                  GtAbstractProperty const& b);

private:
    GtPropertyConversionRegistry();
    ~GtPropertyConversionRegistry() = default;

    QList<GtPropertyConverter> m_canConvertList;

    void registerBasicPropertyConvertersImpl();
};

GT_DATAMODEL_EXPORT GtPropertyConversionRegistry& gtPropConversion();

#endif // GT_PROPERTYCONVERSIONREGISTRY_H
