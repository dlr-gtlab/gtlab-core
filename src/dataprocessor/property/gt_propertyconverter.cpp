/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_propertyconverter.h"
#include <functional>

using CanConnectFunction =
    std::function<bool(GtAbstractProperty& from, GtAbstractProperty& to)>;

GtPropertyConverter::GtPropertyConverter()
{
}
