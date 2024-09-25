/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GT_QVARIANTLISTMODEL_H
#define GT_QVARIANTLISTMODEL_H

#include <gt_listmodel.h>
#include <QVariant>

template<>
inline QVariant gt::asVariant(const QVariant& v)
{
    return v;
}

using QVariantListModel = GtListModel<QVariant>;

#endif // GT_QVARIANTLISTMODEL_H
