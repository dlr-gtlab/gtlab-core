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
    return QVariant::compare(a, b);
#else
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
#endif
}
