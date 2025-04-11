/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 */

#include "gt_qtutilities.h"

#include <QVariant>

int
gt::qvariantCompare(const QVariant &a, const QVariant &b)
{
#if defined(HAVE_QVARIANT_COMPARE_MEMBER)
    return a.compare(b);
#elif defined(HAVE_QVARIANT_COMPARE)
    auto ordering = QVariant::compare(a, b);

    if (ordering < 0) return -1;
    else if (ordering > 0) return 1;
    else return 0;
#else
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
#endif
}

int
gt::metaTypeId(const QVariant &v)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return v.typeId();
#else
    return static_cast<int>(v.type());
#endif
}
