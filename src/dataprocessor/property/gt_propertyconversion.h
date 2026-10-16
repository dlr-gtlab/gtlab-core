/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GT_PROPERTYCONVERSION_H
#define GT_PROPERTYCONVERSION_H

#include "gt_abstractproperty.h"
#include <optional>

namespace gt
{
    namespace conversion
    {
        /**
         * @brief Result status of a property conversion
         *
         * Conversion functions report the outcome of a conversion with
         * this status as part of a ConversionResult.
         */
        enum class conversionSuccess
        {
            /// The value could be converted without loss of information
            Success = 0,
            /// The value was converted, but part of the information was
            /// lost
            Lossy,
            /// The value could not be converted
            Failed
        };

        /**
         * @brief Result of a property conversion
         *
         * Combines the computed target value with the conversion
         * status. The value is only valid if the status is not
         * Failed. Conversion functions only compute the value, the
         * assignment to the target property is done by the receiving
         * property.
         *
         * @tparam T Type of the converted value
         */
        template <typename T>
        struct ConversionResult
        {
            using value_type = T;

            /// Converted value, set for successful and lossy
            /// conversions
            std::optional<T> value;
            /// Status of the conversion
            conversionSuccess status{conversionSuccess::Failed};

            /**
             * @brief Creates a result for a lossless conversion
             * @param value Converted value
             * @return Result with status Success
             */
            static ConversionResult success(T value)
            {
                return {std::move(value), conversionSuccess::Success};
            }

            /**
             * @brief Creates a result for a lossy conversion
             * @param value Converted value
             * @return Result with status Lossy
             */
            static ConversionResult lossy(T value)
            {
                return {std::move(value), conversionSuccess::Lossy};
            }

            /**
             * @brief Creates a result for a failed conversion
             * @return Result without a value and status Failed
             */
            static ConversionResult failed()
            {
                return {std::nullopt, conversionSuccess::Failed};
            }
        };

        /// Type erased conversion result using QVariant as value type
        using erasedConversionResult = ConversionResult<QVariant>;

        /**
         * @brief Type erased property conversion function
         *
         * Takes the source and the target property as const references
         * and computes the value of the target property. The function
         * does not modify the target property itself.
         */
        using convert = std::function<erasedConversionResult(
            const GtAbstractProperty& from, const GtAbstractProperty& to)>;

        /**
         * @brief Wraps a typed conversion callable into a type erased
         * convert function
         *
         * The callable is expected to take both properties as const
         * references and to return a ConversionResult of a type that
         * can be stored in a QVariant, e.g. a lambda returning
         * ConversionResult&lt;int&gt;. The typed value is stored in
         * the erased result as QVariant.
         *
         * @tparam Converter Type of the conversion callable
         * @param converter Conversion callable to wrap
         * @return Type erased convert function
         */
        template <typename Converter>
        gt::conversion::convert eraseConverter(Converter converter)
        {
            return [converter = std::move(converter)](
                       const GtAbstractProperty& from,
                       const GtAbstractProperty& to) mutable
                       -> gt::conversion::erasedConversionResult {
                auto result = converter(from, to);

                if (result.status ==
                        gt::conversion::conversionSuccess::Failed ||
                    !result.value)
                {
                    return gt::conversion::erasedConversionResult::failed();
                }

                using ValueType = typename decltype(result)::value_type;

                QVariant value;

                if constexpr (std::is_same_v<ValueType, QVariant>)
                {
                    value = std::move(*result.value);
                }
                else
                {
                    value = QVariant::fromValue(std::move(*result.value));
                }

                if (result.status == gt::conversion::conversionSuccess::Lossy)
                {
                    return gt::conversion::erasedConversionResult::lossy(
                        std::move(value));
                }

                return gt::conversion::erasedConversionResult::success(
                    std::move(value));
            };
        }

        /**
         * @brief Connection compatibility check function
         *
         * Returns true if the source property may be connected to the
         * target property.
         */
        using isCompatible = std::function<bool(GtAbstractProperty const& from,
                                                GtAbstractProperty const& to)>;

        /**
         * @brief Default conversion for identical value types
         *
         * Copies the value of the source property if both properties
         * provide a valid value of the same variant type.
         *
         * @param source Source property
         * @param target Target property
         * @return Result carrying the source value, or Failed if the
         * variant types of the two properties do not match
         */
        gt::conversion::ConversionResult<QVariant> defaultConverter(
            const GtAbstractProperty& source, const GtAbstractProperty& target);

        /**
         * @brief Creates the default compatibility check
         *
         * The returned function accepts a connection if both
         * properties hold a valid value of the same variant type.
         *
         * @return Default compatibility check function
         */
        gt::conversion::isCompatible defaultCompatibility();

        /**
         * @brief Conversions from a GtDoubleProperty source
         */
        namespace fromDouble
        {
            /**
             * @brief Checks if a double property may be connected to
             * another double property
             *
             * The unit categories of both properties must match (or
             * one of the properties has no unit) and the active value
             * ranges must at least overlap in parts.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToDouble(GtAbstractProperty const& source,
                                    GtAbstractProperty const& target);

            /**
             * @brief Converts the value of a double property to int
             *
             * The value is rounded to the nearest integer. The
             * conversion fails for NaN or Inf values, for values
             * outside the range of int, and for values outside the
             * active boundaries of the target property. Loss of the
             * decimal part or of the unit information is reported as
             * Lossy.
             *
             * @param source Source property
             * @param target Target property
             * @return Result carrying the converted value
             */
            ConversionResult<int> toInt(GtAbstractProperty const& source,
                                        GtAbstractProperty const& target);

            /**
             * @brief Checks if a double property may be connected to
             * an int property
             *
             * Always returns true, the range is validated by the
             * conversion function.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);


        } // namespace fromDouble

        /**
         * @brief Conversions from a GtIntProperty source
         */
        namespace fromInt
        {
            /**
             * @brief Checks if an int property may be connected to
             * another int property
             *
             * The active value ranges of both properties must at
             * least overlap in parts.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);

            /**
             * @brief Converts the value of an int property to double
             *
             * The value is converted without loss of information.
             *
             * @param source Source property
             * @param target Target property
             * @return Result carrying the converted value
             */
            ConversionResult<double> toDouble(GtAbstractProperty const& source,
                                              GtAbstractProperty const& target);

            /**
             * @brief Checks if an int property may be connected to a
             * double property
             *
             * Always returns true.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToDouble(GtAbstractProperty const& source,
                                    GtAbstractProperty const& target);

            /**
             * @brief Checks if an int property may be connected to a
             * bool property
             *
             * Always returns true.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToBool(GtAbstractProperty const& source,
                                  GtAbstractProperty const& target);

            /**
             * @brief Converts the value of an int property to bool
             *
             * Zero maps to false, all other values to true. Values
             * other than zero and one are reported as Lossy.
             *
             * @param source Source property
             * @param target Target property
             * @return Result carrying the converted value
             */
            ConversionResult<bool> toBool(GtAbstractProperty const& source,
                                          GtAbstractProperty const& target);

        } // namespace fromInt

        /**
         * @brief Conversions from a GtBoolProperty source
         */
        namespace fromBool
        {
            /**
             * @brief Checks if a bool property may be connected to an
             * int property
             *
             * Always returns true.
             *
             * @param source Source property
             * @param target Target property
             * @return True if the connection is allowed
             */
            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);

            /**
             * @brief Converts the value of a bool property to int
             *
             * False maps to zero, true maps to one. The value is
             * converted without loss of information.
             *
             * @param source Source property
             * @param target Target property
             * @return Result carrying the converted value
             */
            ConversionResult<int> toInt(GtAbstractProperty const& source,
                                        GtAbstractProperty const& target);
        } // namespace fromBool
    } // namespace conversion
} // namespace gt

#endif // GT_PROPERTYCONVERSION_H
