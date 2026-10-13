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

        using convert = std::function<conversionSuccess(
            GtAbstractProperty const& from, GtAbstractProperty& to)>;

        using canConnect = std::function<bool(GtAbstractProperty const& from,
                                              GtAbstractProperty const& to)>;

        /**
         * @brief defaultConverter uses basic conversion from source to target
         * for identical types
         * @param source
         * @param target
         * @return
         */
        conversionSuccess defaultConverter(GtAbstractProperty const& source,
                                           GtAbstractProperty& target);

        namespace fromDouble
        {
            bool canConnectToDouble(
                GtAbstractProperty const& source,
                GtAbstractProperty const& target);

            conversionSuccess toInt(
                GtAbstractProperty const& source,
                GtAbstractProperty& target);

            bool canConnectToInt(
                GtAbstractProperty const& source,
                GtAbstractProperty const& target);


        } // namespace fromDouble

        namespace fromInt
        {
            bool canConnectToInt(
                GtAbstractProperty const& source,
                GtAbstractProperty const& target);

            conversionSuccess toDouble(
                GtAbstractProperty const& source,
                GtAbstractProperty& target);

            bool canConnectToDouble(
                GtAbstractProperty const& source,
                GtAbstractProperty const& target);
        } // namespace fromInt

    } // namespace conversion
} // namespace gt

#endif // GT_PROPERTYCONVERSION_H
