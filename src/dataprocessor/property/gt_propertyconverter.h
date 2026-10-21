/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GT_PROPERTYCONVERTER_H
#define GT_PROPERTYCONVERTER_H

#include "gt_datamodel_exports.h"
#include "gt_propertyconversion.h"


/**
 * @brief Conversion between the value types of two property classes
 *
 * A converter holds the meta objects of a source and a target property
 * class together with the function that decides whether two concrete
 * property instances of these classes may be connected
 * (gt::conversion::isCompatible) and the function that computes the
 * target value from the source property (gt::conversion::convert).
 *
 * Converter instances are created by GtPropertyConversionRegistry when
 * a converter is registered for a source/target pair. The conversion
 * function is mandatory, the compatibility function is optional.
 */
class GT_DATAMODEL_EXPORT GtPropertyConverter
{
public:
    /**
     * @brief Creates a converter for a source/target property class pair
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @param conversion Function computing the target value from the source
     * property. The registry rejects the registration of empty functions.
     * @param isCompatible Function deciding whether two concrete property
     * instances may be connected. If empty, the connection check falls
     * back to the implicit default compatibility of the pair.
     */
    GtPropertyConverter(QMetaObject from, QMetaObject to,
                        gt::conversion::convert conversion,
                        gt::conversion::isCompatible isCompatible = {});

    /**
     * @brief Creates a converter for a source/target property class pair
     * from a typed conversion callable
     *
     * The callable is type-erased with gt::conversion::eraseConverter,
     * so e.g. a lambda returning
     * gt::conversion::ConversionResult&lt;int&gt; can be passed directly.
     *
     * @tparam Converter Type of the conversion callable. Must be invocable
     * with two const GtAbstractProperty& and not already be the type
     * erased gt::conversion::convert.
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @param conversion Typed conversion callable to type-erase
     * @param isCompatible Function deciding whether two concrete property
     * instances may be connected. If empty, the connection check falls
     * back to the implicit default compatibility of the pair.
     */
    template <typename Converter,
              std::enable_if_t<
                  std::is_invocable_v<Converter&, const GtAbstractProperty&,
                                      const GtAbstractProperty&> &&
                      !std::is_same_v<std::decay_t<Converter>,
                                      gt::conversion::convert>,
                  int> = 0>
    GtPropertyConverter(QMetaObject from, QMetaObject to,
                        Converter&& conversion,
                        gt::conversion::isCompatible isCompatible = {}) :
        GtPropertyConverter(
            std::move(from), std::move(to),
            gt::conversion::eraseConverter(std::forward<Converter>(conversion)),
            std::move(isCompatible))
    {
    }

    /**
     * @brief Returns the class name of the source property class
     * @return Class name of the source property class
     */
    QString fromClassName() const;

    /**
     * @brief Returns the class name of the target property class
     * @return Class name of the target property class
     */
    QString toClassName() const;

    /**
     * @brief Returns the compatibility function of the converter
     * @return Function deciding whether two concrete property instances
     * may be connected, or an empty function if none was registered.
     */
    gt::conversion::isCompatible isCompatibleFunc() const;

    /**
     * @brief Returns the conversion function of the converter
     * @return Function computing the target value from the source property
     */
    gt::conversion::convert convertFunc() const;

    /**
     * @brief Returns whether a compatibility function was registered
     * @return true if the compatibility function is not empty
     */
    bool isCompatibleDefined() const;

    /**
     * @brief Returns whether a conversion function was registered
     * @return true if the conversion function is not empty
     */
    bool conversionDefined() const;

private:
    /// Meta object of the source property class
    QMetaObject m_from;

    /// Meta object of the target property class
    QMetaObject m_to;

    /// Function deciding whether two concrete property instances may be
    /// connected. May be empty.
    gt::conversion::isCompatible m_isCompatibleFunc;

    /// Function computing the target value from the source property
    gt::conversion::convert m_conversion;
};

#endif // GT_PROPERTYCONVERTER_H
