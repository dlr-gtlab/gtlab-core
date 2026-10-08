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
#include "gt_abstractproperty.h"

#include <functional>
#include <qobjectdefs.h>
struct GT_DATAMODEL_EXPORT GtPropertyConverter
{
    using CanConnectFunction =
        std::function<bool(GtAbstractProperty& from, GtAbstractProperty& to)>;
    QMetaObject to;
    CanConnectFunction f;
};

#endif // GT_PROPERTYCONVERTER_H
