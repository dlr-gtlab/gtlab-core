/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_propertyconversion.h"

#include "gt_propertyconversionregistry.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"

void
gt::conversion::registerBasicPropertyConverters()
{
    // add the basic converter registrations here

    // converter from double to int
    gtPropConversion().registerConnectionCompatibility(
        GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        // conversion
        [](GtAbstractProperty const& a, // LCOV_EXCL_LINE
           GtAbstractProperty& b) {
            const auto& from = static_cast<const GtDoubleProperty&>(a);
            auto& to = static_cast<GtIntProperty&>(b);

            double baseValue = from.getVal();

            const double rounded = std::round(baseValue);

            int result = static_cast<int>(rounded);

            // Abweichung vom ursprünglichen double
            const double error = std::abs(baseValue - rounded);

            // Toleranz entsprechend der Größenordnung des Wertes
            const double tolerance = std::numeric_limits<double>::epsilon() *
                                     std::max(1.0, std::abs(baseValue));

            to.setVal(result);

            if (error <= tolerance)
            {
                return gt::conversion::conversionSuccess::Success;
            }

            return gt::conversion::conversionSuccess::Lossy;
        },
        // can connect
        [](GtAbstractProperty const&, // LCOV_EXCL_LINE
           GtAbstractProperty const&) { return true; });


    // converter from int to double
    gtPropConversion().registerConnectionCompatibility(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject,
        // conversion
        [](GtAbstractProperty const& a, // LCOV_EXCL_LINE
           GtAbstractProperty& b) {
            const auto& from = static_cast<const GtIntProperty&>(a);
            auto& to = static_cast<GtDoubleProperty&>(b);

            double res = double(from.getVal());

            to.setVal(res);

            return gt::conversion::conversionSuccess::Success;
        },
        // can connect
        [](GtAbstractProperty const&, // LCOV_EXCL_LINE
           GtAbstractProperty const&) { return true; });
}
