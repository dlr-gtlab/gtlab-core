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

/**
 * @brief Registry for connection compatibility checks and conversion
 * functions between property classes
 *
 * Modules register, for a pair of a source and a target property class,
 * the function that computes the target value from the source property
 * and, optionally, the function that decides whether two concrete
 * property instances may be connected.
 *
 * The registry is a singleton. Converters can be registered for
 * arbitrary property class pairs, e.g. in the init() function of a
 * module.
 *
 * Lookups for a source/target pair consider the inheritance chains of
 * both property classes. If a converter is registered for the exact
 * pair, it is used. Otherwise, the most specific converter registered
 * for a pair of common base classes is used, i.e. the one with the
 * smallest sum of the inheritance depths of the source and the target
 * class; if several converters match at the same depth, the more
 * specific source class wins, then the more specific target class.
 *
 * If no converter is registered for a pair and both property classes
 * share a common base property class, the pair is handled by an
 * implicit default converter. It accepts a value if both property
 * instances hold the same variant type and passes the value through
 * unchanged (see gt::conversion::defaultConverter and
 * gt::conversion::defaultCompatibility).
 */
class GT_DATAMODEL_EXPORT GtPropertyConversionRegistry
{

public:
    // Delete copy and assignment functions
    GtPropertyConversionRegistry(const GtPropertyConversionRegistry&) = delete;

    GtPropertyConversionRegistry& operator=(
        const GtPropertyConversionRegistry&) = delete;

    /**
     * @brief Returns a reference to the singleton registry instance
     * @return Reference to the registry
     */
    static GtPropertyConversionRegistry& getInstance();

    /**
     * @brief Registers a converter for a source/target property class
     * pair
     *
     * Only one converter can be registered per source/target pair, a
     * second registration for the same pair is ignored with a warning.
     * The conversion function must not be empty, otherwise the
     * registration is ignored with a warning. The compatibility
     * function may be empty.
     *
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @param convert Function computing the target value from the source
     * property. Must not be empty.
     * @param canConnect Function deciding whether two concrete property
     * instances may be connected. May be empty.
     */
    void registerConnectionCompatibility(
        QMetaObject from, QMetaObject to, gt::conversion::convert convert,
        gt::conversion::isCompatible canConnect = {});

    /**
     * @brief Registers a converter for a source/target property class
     * pair from a typed conversion callable
     *
     * The callable is type-erased with gt::conversion::eraseConverter
     * and forwarded to the other overload, so e.g. a lambda returning
     * gt::conversion::ConversionResult&lt;int&gt; can be passed
     * directly.
     *
     * @tparam Converter Type of the conversion callable
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @param conversion Typed conversion callable to type-erase
     * @param canConnect Function deciding whether two concrete property
     * instances may be connected. May be empty.
     */
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
     * @brief Returns the compatibility check of a source/target property
     * class pair
     *
     * The inheritance chains of both property classes are considered,
     * see the class documentation.
     *
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @return The compatibility check of the most specific matching
     * converter. If the matching converter does not provide one, the
     * implicit default compatibility check is returned. If no converter
     * matches at all, an empty function is returned.
     */
    gt::conversion::isCompatible isCompatibleFunction(
        QMetaObject const& from, QMetaObject const& to) const;

    /**
     * @brief Returns the conversion function of a source/target property
     * class pair
     *
     * The inheritance chains of both property classes are considered,
     * see the class documentation.
     *
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @return The conversion function of the most specific matching
     * converter. If the matching converter does not provide one, the
     * implicit default conversion function is returned. If no converter
     * matches at all, an empty function is returned.
     */
    gt::conversion::convert convertFunction(QMetaObject const& from,
                                            QMetaObject const& to) const;

    /**
     * @brief Finds the most specific explicitly registered converter
     * for a source/target property class pair
     *
     * The inheritance chains of both property classes are considered,
     * see the class documentation. The implicit default converter is
     * not considered.
     *
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @return Pointer to the most specific matching converter, or
     * nullptr if none is registered.
     */
    const GtPropertyConverter* findConverterWithInheritance(
        const QMetaObject& from, const QMetaObject& to) const;


    /**
     * @brief Returns the target property classes a converter is
     * registered for
     *
     * @param from Class name of the source property class
     * @return Class names of the target property classes for which a
     * converter is registered from the given source class
     */
    QStringList converterAvailable(const QString& from) const;

    /**
     * @brief Checks if the source property may be connected to the
     * target property
     *
     * The compatibility check of the most specific matching converter
     * is used. If no compatibility check matches at all, the class
     * names of the two properties are compared.
     *
     * @param a Source property
     * @param b Target property
     * @return True if the source property may be connected to the
     * target property
     */
    bool connectionCheckAvailable(GtAbstractProperty const& a,
                                  GtAbstractProperty const& b);

private:
    /// Purpose of a converter lookup
    enum class LookupPurpose
    {
        /// Any registered converter matches
        AnyRegistered,
        /// Only converters providing a conversion function match
        Conversion,
        /// Only converters providing a compatibility function match
        Compatibility
    };

    /// Result of a converter lookup
    struct ConverterMatch
    {
        /// Best matching converter, nullptr marks the implicit default
        /// converter
        const GtPropertyConverter* converter = nullptr;
        /// True if a matching converter was found
        bool found = false;
        /// Index of the source class in the inheritance chain of the
        /// lookup
        std::size_t fromIndex = 0;
        /// Index of the target class in the inheritance chain of the
        /// lookup
        std::size_t toIndex = 0;
    };

    /**
     * @brief Finds the most specific converter matching a source/target
     * property class pair
     *
     * For each registered converter, the indices of the source and
     * target class in the inheritance chains of the lookup classes are
     * determined. The converter with the smallest sum of both indices
     * is the most specific match; if several converters match with the
     * same sum, the smaller source index wins, then the smaller target
     * index. An explicitly registered converter beats the implicit
     * default converter at the same position.
     *
     * @param from Meta object of the source property class
     * @param to Meta object of the target property class
     * @param purpose Determines which functions a matching converter
     * must provide
     * @param includeImplicit If true, the implicit default converter
     * is considered for pairs of classes sharing a common base
     * property class
     * @return The best matching converter with its position in the
     * inheritance chains
     */
    ConverterMatch findBestConverterWithInheritance(
        const QMetaObject& from, const QMetaObject& to, LookupPurpose purpose,
        bool includeImplicit = true) const;

    /// Creates the registry instance and registers the converters of
    /// the built-in property classes
    GtPropertyConversionRegistry();
    ~GtPropertyConversionRegistry() = default;

    /// Converters registered for source/target property class pairs
    QList<GtPropertyConverter> m_canConvertList;

    /// Registers the converters of the built-in property classes
    void registerBasicPropertyConvertersImpl();
};

/**
 * @brief Returns a reference to the singleton registry instance
 *
 * Convenience function for GtPropertyConversionRegistry::getInstance().
 *
 * @return Reference to the registry
 */
GT_DATAMODEL_EXPORT GtPropertyConversionRegistry& gtPropConversion();

#endif // GT_PROPERTYCONVERSIONREGISTRY_H
