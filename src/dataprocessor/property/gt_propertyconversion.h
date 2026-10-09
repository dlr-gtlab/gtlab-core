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

    } // namespace conversion
} // namespace gt

#endif // GT_PROPERTYCONVERSION_H
