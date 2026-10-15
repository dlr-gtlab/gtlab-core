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
        enum class conversionSuccess
        {
            Success = 0,
            Lossy,
            Failed
        };

        template <typename T>
        struct ConversionResult
        {
            using value_type = T;

            std::optional<T> value;
            conversionSuccess status{conversionSuccess::Failed};

            static ConversionResult success(T value)
            {
                return {std::move(value), conversionSuccess::Success};
            }

            static ConversionResult lossy(T value)
            {
                return {std::move(value), conversionSuccess::Lossy};
            }

            static ConversionResult failed()
            {
                return {std::nullopt, conversionSuccess::Failed};
            }
        };

        using erasedConversionResult = ConversionResult<QVariant>;

        using convert = std::function<erasedConversionResult(
            const GtAbstractProperty& from, const GtAbstractProperty& to)>;

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

        using isCompatible = std::function<bool(GtAbstractProperty const& from,
                                                GtAbstractProperty const& to)>;

        /**
         * @brief defaultConverter uses basic conversion from source to target
         * for identical types
         * @param source
         * @param target
         * @return
         */
        gt::conversion::ConversionResult<QVariant> defaultConverter(
            const GtAbstractProperty& source, const GtAbstractProperty& target);

        gt::conversion::isCompatible defaultCompatibility();

        namespace fromDouble
        {
            bool canConnectToDouble(GtAbstractProperty const& source,
                                    GtAbstractProperty const& target);

            ConversionResult<int> toInt(GtAbstractProperty const& source,
                                        GtAbstractProperty const& target);

            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);


        } // namespace fromDouble

        namespace fromInt
        {
            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);

            ConversionResult<double> toDouble(GtAbstractProperty const& source,
                                              GtAbstractProperty const& target);

            bool canConnectToDouble(GtAbstractProperty const& source,
                                    GtAbstractProperty const& target);

            bool canConnectToBool(GtAbstractProperty const& source,
                                  GtAbstractProperty const& target);

            ConversionResult<bool> toBool(GtAbstractProperty const& source,
                                          GtAbstractProperty const& target);

        } // namespace fromInt

        namespace fromBool
        {
            bool canConnectToInt(GtAbstractProperty const& source,
                                 GtAbstractProperty const& target);

            ConversionResult<int> toInt(GtAbstractProperty const& source,
                                        GtAbstractProperty const& target);
        } // namespace fromBool
    } // namespace conversion
} // namespace gt

#endif // GT_PROPERTYCONVERSION_H
